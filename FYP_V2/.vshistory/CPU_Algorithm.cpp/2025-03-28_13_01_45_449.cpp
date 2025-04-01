#include "pch.h"
#include <iostream>
#include <vector>
#include <climits>
#include <queue>
#include <stack>
#include <omp.h>
#include "CPU_Algorithm.h";
#include "UnionFind.h";

std::vector<int> dijkstra(const Graph& graph, int src) {
	int numOfVertices = graph.getNumOfVertices();
	int** adjMatrix = graph.getAdjacencyMatrix();

	std::vector<int> dist(numOfVertices, INT_MAX);
	std::vector<bool> visited(numOfVertices, false);

	dist[src] = 0;

	for (int i = 0; i < numOfVertices; i++) {
		int currentMinDist = INT_MAX;
		int currentVertexIdx = -1;

		for (int j = 0; j < numOfVertices; j++) {
			if (!visited[j] && dist[j] <= currentMinDist) {
				currentMinDist = dist[j];
				currentVertexIdx = j;
			}
		}

		visited[currentVertexIdx] = true;

		for (int k = 0; k < numOfVertices; k++) {
			int currentDist = dist[currentVertexIdx] + adjMatrix[currentVertexIdx][k];
			if (!visited[k] && adjMatrix[currentVertexIdx][k] != 0 && dist[currentVertexIdx] != INT_MAX) {
				if (currentDist < dist[k]) {
					dist[k] = currentDist;
				}
			}
		}
	}

	return dist;
}
std::vector<std::vector<int>> CPU_Algorithm::CPU_Parallel_Dijkstra(const Graph& graph) {
	std::vector<std::vector<int>> allDistances(graph.getNumOfVertices(), std::vector<int>(graph.getNumOfVertices(), INT_MAX));

	#pragma omp parallel for schedule(dynamic)
	for (int i = 0; i < graph.getNumOfVertices(); i++) {
		allDistances[i] = dijkstra(graph, i);
	}

	return allDistances;
}

std::vector<std::vector<int>> CPU_Algorithm::CPU_2DBlockMappedFW(const Graph& graph) {
	int numOfVertices = graph.getNumOfVertices();
	int** adjMatrix = graph.getAdjacencyMatrix();
	std::vector<std::vector<int>> allDistances(numOfVertices, std::vector<int>(numOfVertices));

	for (int i = 0; i < numOfVertices; ++i) {
		for (int j = 0; j < numOfVertices; ++j) {
			allDistances[i][j] = (i == j) ? 0 : (adjMatrix[i][j] ? adjMatrix[i][j] : INT_MAX);
		}
	}

	const int block_size = 32;

	for (int k = 0; k < numOfVertices; k++) {
		int blockIdx_x = (numOfVertices + block_size - 1) / block_size;
		int blockIdx_y = (numOfVertices + block_size - 1) / block_size;

		#pragma omp parallel for collapse(2)
		for (int y = 0; y < blockIdx_y; y++) {
			for (int x = 0; x < blockIdx_x; x++) {

				int i_start = y * block_size;
				int i_end = min(i_start + block_size, numOfVertices);
				int j_start = x * block_size;
				int j_end = min(j_start + block_size, numOfVertices);

				std::vector<int> ik_values(i_end - i_start);
				std::vector<int> kj_values(j_end - j_start);

				for (int i = i_start; i < i_end; ++i) {
					ik_values[i - i_start] = allDistances[i][k];
				}

				for (int j = j_start; j < j_end; ++j) {
					kj_values[j - j_start] = allDistances[k][j];
				}

				for (int i = i_start; i < i_end; ++i) {
					for (int j = j_start; j < j_end; ++j) {
						int ik = ik_values[i - i_start];
						int kj = kj_values[j - j_start];
						if (ik != INT_MAX && kj != INT_MAX && allDistances[i][j] > ik + kj) {
							allDistances[i][j] = ik + kj;
						}
					}
				}
			}
		}
	}

	return allDistances;
}

std::vector<int> CPU_Algorithm::CPU_2DPartition_BFS(const Graph& graph, int src) {
	int numOfVertices = graph.getNumOfVertices();
	int** adjMatrix = graph.getAdjacencyMatrix();

	int width = (int)sqrt(numOfVertices);
	int blockSize = 32;
	int numOfBlocks = (width + blockSize - 1) / blockSize;

	std::vector<std::pair<int, int>> subMatrixIdx(numOfVertices);
	for (int i = 0; i < numOfVertices; i++) {
		int row = i / width;
		int col = i % width;
		subMatrixIdx[i] = std::make_pair(row / blockSize, col / blockSize);
	}

	std::vector<int> depth(numOfVertices, -1);
	std::vector<std::vector<std::queue<int>>> subMatrix(numOfBlocks, std::vector<std::queue<int>>(numOfBlocks));
	std::vector<std::vector<omp_lock_t>> matrixLock(numOfBlocks, std::vector<omp_lock_t>(numOfBlocks));

	for (auto& lockVector : matrixLock) {
		for (auto& lock : lockVector) {
			omp_init_lock(&lock);
		}
	}

	depth[src] = 0;
	auto [subMatrixkRow, subMatrixCol] = subMatrixIdx[src];
	omp_set_lock(&matrixLock[subMatrixkRow][subMatrixCol]);
	subMatrix[subMatrixkRow][subMatrixCol].push(src);
	omp_unset_lock(&matrixLock[subMatrixkRow][subMatrixCol]);

	bool hasUnprocessedVertices = true;
	while (hasUnprocessedVertices) {
		hasUnprocessedVertices = false;

		#pragma omp parallel for schedule(dynamic) collapse(2)
		for (int i = 0; i < numOfBlocks; i++) {
			for (int j = 0; j < numOfBlocks; j++) {
				omp_set_lock(&matrixLock[i][j]);
				if (!subMatrix[i][j].empty()) {
					std::queue<int> currentQueue = subMatrix[i][j];
					omp_unset_lock(&matrixLock[i][j]);

					if (currentQueue.empty()) {
						continue;
					}

					int currentVertex = subMatrix[i][j].front();
					subMatrix[i][j].pop();


					for (int k = 0; k < numOfVertices; k++) {
						if (adjMatrix[currentVertex][k] != 0 && depth[k] == -1) {
							depth[k] = depth[currentVertex] + 1;
							auto [subMatrixkRow, subMatrixCol] = subMatrixIdx[k];
							omp_set_lock(&matrixLock[subMatrixkRow][subMatrixCol]);
							subMatrix[subMatrixkRow][subMatrixCol].push(k);
							omp_unset_lock(&matrixLock[subMatrixkRow][subMatrixCol]);

							#pragma omp atomic write
							hasUnprocessedVertices = true;
						}
					}
				}
				else {
					omp_unset_lock(&matrixLock[i][j]);
				}
			}
		}
	}
	for (std::vector<omp_lock_t> lockVector : matrixLock) {
		for (omp_lock_t lock : lockVector) {
			omp_destroy_lock(&lock);
		}
	}

	return depth;
}

std::vector<Edge> BasicKruskal(std::vector<Edge>& edges, UnionFind& unionFind) {
	std::vector<Edge> result;
	for (Edge edge : edges) {
		int setA = unionFind.find(edge.from);
		int setB = unionFind.find(edge.to);
		if (setA != setB) {
			unionFind.unite(setA, setB);
			result.push_back(edge);
		}
	}
	return result;
}

std::vector<Edge> FilterKruskal_Recursion(std::vector<Edge>& edges, UnionFind& unionFind, int threshold) {
	std::sort(edges.begin(), edges.end(), [](const Edge& e1, const Edge& e2) {
		return e1.weight < e2.weight;
	});

	if (edges.size() < threshold) {
		return BasicKruskal(edges, unionFind);
	}

	int medianWeight = edges[edges.size() / 2].weight;

	std::vector<Edge> leftEdges, rightEdges;

	#pragma omp parallel 
	{
		std::vector<Edge> local_Left, local_Right;
		#pragma omp for 
		for (size_t i = 0; i < edges.size(); i++) {
			if (edges[i].weight < medianWeight) {
				local_Left.push_back(edges[i]);
			}
			else {
				local_Right.push_back(edges[i]);
			}
		}
		#pragma omp critical
		{
			leftEdges.insert(leftEdges.end(), local_Left.begin(), local_Left.end());
			rightEdges.insert(rightEdges.end(), local_Right.begin(), local_Right.end());
		}
	}

	std::vector<Edge> leftMST;
	#pragma omp task shared(leftMST)
	{
		leftMST = FilterKruskal_Recursion(leftEdges, unionFind, threshold);
	}
	#pragma omp taskwait

	std::vector<Edge> filteredRight;
	for (Edge& edge : rightEdges) {
		if (unionFind.find(edge.from) != unionFind.find(edge.to)) {
			filteredRight.push_back(edge);
		}
	}
	std::vector<Edge> rightMST = FilterKruskal_Recursion(filteredRight, unionFind, threshold);

	leftMST.insert(leftMST.end(), rightMST.begin(), rightMST.end());
	return leftMST;
}

std::vector<Edge> CPU_Algorithm::CPU_Parallel_FilterKruskal(Graph& graph) {
	std::vector<Edge> edges = graph.getUndirectedEdges();
	UnionFind unionFind(graph.getNumOfVertices());
	const int threshold = 500;
	return FilterKruskal_Recursion(edges, unionFind, threshold);
}