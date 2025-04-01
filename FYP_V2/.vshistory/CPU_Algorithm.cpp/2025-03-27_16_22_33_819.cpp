#include "pch.h"
#include <iostream>
#include <vector>
#include <climits>
#include <queue>
#include <stack>
#include <omp.h>
#include "CPU_Algorithm.h";

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

std::vector<std::vector<int>> CPU_Algorithm::CPU_Kosaraju(Graph& graph) {
	int numOfVertices = graph.getNumOfVertices();
	std::vector<int> compileOrder;
	std::vector<std::atomic<bool>> visited(numOfVertices);

	for (auto& v : visited) v.store(false, std::memory_order_relaxed);

	for (int i = 0; i < numOfVertices; i++) {
		if (!visited[i]) {
			std::stack<int> stack;
			stack.push(i);
			visited[i].store(true, std::memory_order_relaxed);

			while (!stack.empty()) {
				int currentVertex = stack.top();
				bool hasUnvisitedVertex = false;
				for (int j = 0; j < numOfVertices; j++) {
					if (graph.getAdjacencyMatrix()[currentVertex][j] != 0 && !visited[j]) {
						stack.push(j);
						visited[j].store(true, std::memory_order_relaxed);
						hasUnvisitedVertex = true;
						break;
					}
				}

				if (!hasUnvisitedVertex) {
					stack.pop();
					compileOrder.push_back(currentVertex);
				}
			}
		}
	}

	Graph reversedGraph = graph.getReversedGraph();
	std::vector<std::vector<int>> sccs;

	for (auto& v : visited) v.store(false, std::memory_order_relaxed);

	#pragma omp parallel
	{
		#pragma omp single
		for (auto iter = compileOrder.rbegin(); iter != compileOrder.rend(); iter++) {
			int currentVertex = *iter;
			if (!visited[currentVertex]) {
				#pragma omp task shared(processed, sccs)
				{
					std::vector<int> scc;
					std::stack<int> stack;
					stack.push(currentVertex);
					visited[currentVertex] = true;

					while (!stack.empty()) {
						int currentV = stack.top();
						stack.pop();
						scc.push_back(currentV);
						for (int j = 0; j < numOfVertices; ++j) {
							if (reversedGraph.getAdjacencyMatrix()[currentV][j] != 0 && !visited[j]) {
								#pragma omp atomic write
								visited[j] = true;
								stack.push(j);
							}
						}
					}

					#pragma omp critical
					sccs.push_back(scc);
				}
			}
		}
	}

	return sccs;
}