#include <iostream>
#include <vector>
#include <stack>
#include <unordered_map>
#include <climits>
#include <thrust/sequence.h>
#include <thrust/sort.h>
#include <thrust/partition.h>
#include <thrust/execution_policy.h>
#include <cub/cub.cuh>
#include <cuda.h>
#include <cuda_runtime.h>
#include "device_launch_parameters.h"
#include "GPU_Algorithm.cuh"

__global__ void dijkstraKernel(const int* adjMatrix, int* distMatrix, int numOfVertices) {
    int src = blockIdx.x * blockDim.x + threadIdx.x;
    if (src >= numOfVertices) return;

    int* dist = new int[numOfVertices];
    bool* visited = new bool[numOfVertices];

    for (int i = 0; i < numOfVertices; i++) {
        dist[i] = INT_MAX;
        visited[i] = false;
    }

    dist[src] = 0;

    for (int i = 0; i < numOfVertices; i++) {
        int currentMinDist = INT_MAX;
        int currentVertexIdx = -1;

        for (int j = 0; j < numOfVertices; j++) {
            if (!visited[j] && dist[j] < currentMinDist) {
                currentMinDist = dist[j];
                currentVertexIdx = j;
            }
        }
        if (currentVertexIdx == -1) break;
        visited[currentVertexIdx] = true;

        for (int k = 0; k < numOfVertices; k++) {
            int weight = adjMatrix[currentVertexIdx * numOfVertices + k];
            if (weight != 0 && !visited[k] && dist[currentVertexIdx] != INT_MAX) {
				dist[k] = dist[k] < dist[currentVertexIdx] + weight ? dist[k] : dist[currentVertexIdx] + weight;
            }
        }
    }

    for (int i = 0; i < numOfVertices; i++) {
        distMatrix[src * numOfVertices + i] = dist[i];
    }

    delete[] dist;
    delete[] visited;
}

std::vector<std::vector<int>> GPU_Algorithm::GPU_Parallel_Dijkstra(const Graph& graph) {
    int numOfVertices = graph.getNumOfVertices();
    int** h_adjMatrix = graph.getAdjacencyMatrix();

    std::vector<int> _1DAdjMatrix(numOfVertices * numOfVertices);
    for (int i = 0; i < numOfVertices; i++) {
        for (int j = 0; j < numOfVertices; j++) {
            _1DAdjMatrix[i * numOfVertices + j] = h_adjMatrix[i][j];
        }
    }

    int* d_AdjMatrix;
	int* d_Dist;
    cudaMalloc(&d_AdjMatrix, numOfVertices * numOfVertices * sizeof(int));
    cudaMalloc(&d_Dist, numOfVertices * numOfVertices * sizeof(int));
    cudaMemcpy(d_AdjMatrix, _1DAdjMatrix.data(), numOfVertices * numOfVertices * sizeof(int), cudaMemcpyHostToDevice);

    int blockSize, minGridSize;
    cudaOccupancyMaxPotentialBlockSize(
        &minGridSize,
        &blockSize,
        dijkstraKernel,
        0,
        0
    );

    int gridSize = (numOfVertices + blockSize - 1) / blockSize;

    dijkstraKernel<<<gridSize, blockSize>>> (d_AdjMatrix, d_Dist, numOfVertices);

    std::vector<int> h_Dist(numOfVertices * numOfVertices);
    cudaMemcpy(h_Dist.data(), d_Dist, numOfVertices * numOfVertices * sizeof(int), cudaMemcpyDeviceToHost);

    std::vector<std::vector<int>> allDistances(numOfVertices, std::vector<int>(numOfVertices));
    for (int i = 0; i < numOfVertices; i++) {
        for (int j = 0; j < numOfVertices; j++) {
            allDistances[i][j] = h_Dist[i * numOfVertices + j];
        }
    }

    cudaFree(d_AdjMatrix);
    cudaFree(d_Dist);

    return allDistances;
}

__global__ void floydWarshallKernel(int* _1DadjMatrix, int numOfVertices, int k) {
    int i = blockIdx.y * blockDim.y + threadIdx.y;
    int j = blockIdx.x * blockDim.x + threadIdx.x;

    if (i >= numOfVertices || j >= numOfVertices) return;

    int ij = i * numOfVertices + j;
    int ik = i * numOfVertices + k;
    int kj = k * numOfVertices + j;

    int dist_ik = _1DadjMatrix[ik];
    int dist_kj = _1DadjMatrix[kj];
    int current_dist = _1DadjMatrix[ij];

    if (dist_ik != INT_MAX && dist_kj != INT_MAX) {
        int new_dist = dist_ik + dist_kj;
        if (current_dist > new_dist) {
            _1DadjMatrix[ij] = new_dist;
        }
    }
}

std::vector<std::vector<int>> GPU_Algorithm::GPU_2DBlockMappedFW(const Graph& graph) {
	int numOfVertices = graph.getNumOfVertices();
	int** adjMatrix = graph.getAdjacencyMatrix();

	std::vector<int> _1DAdjMatrix(numOfVertices * numOfVertices);
	for (int i = 0; i < numOfVertices; i++) {
		for (int j = 0; j < numOfVertices; j++) {
			_1DAdjMatrix[i * numOfVertices + j] = (i == j) ? 0 : (adjMatrix[i][j] ? adjMatrix[i][j] : INT_MAX);
		}
	}

    int* d_adjMatrix;
    cudaMalloc(&d_adjMatrix, numOfVertices * numOfVertices * sizeof(int));
    cudaMemcpy(d_adjMatrix, _1DAdjMatrix.data(), numOfVertices * numOfVertices * sizeof(int), cudaMemcpyHostToDevice);

	const int block_Size = 32;

	dim3 blockSize(block_Size, block_Size);
	dim3 grid = dim3((numOfVertices + block_Size - 1) / block_Size, (numOfVertices + block_Size - 1) / block_Size);

    for (int k = 0; k < numOfVertices; k++) {
		floydWarshallKernel <<<grid, blockSize>>> (d_adjMatrix, numOfVertices, k);
    }

    cudaMemcpy(_1DAdjMatrix.data(), d_adjMatrix, numOfVertices * numOfVertices * sizeof(int), cudaMemcpyDeviceToHost);
    cudaFree(d_adjMatrix);

    std::vector<std::vector<int>> result(numOfVertices, std::vector<int>(numOfVertices));
    for (int i = 0; i < numOfVertices; i++) {
        for (int j = 0; j < numOfVertices; j++) {
            result[i][j] = _1DAdjMatrix[i * numOfVertices + j];
        }
    }

    return result;
}

__global__ void bfs2DKernel(
    const int* adjMatrix,
    int* depth,
    const int2* subMatrixIdx,
    int numOfVertices,
    int width,
    int blockSize,
    int* currentFrontier,
    int* currentFrontierSize,
    int* nextFrontier,
    int* nextFrontierSize
) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx >= *currentFrontierSize) return;

    int current_vertex = currentFrontier[idx];
    int2 sub_matrix_idx = subMatrixIdx[current_vertex];

    for (int v = sub_matrix_idx.x * blockSize; v < (sub_matrix_idx.x + 1) * blockSize; v++) {
        for (int w = sub_matrix_idx.y * blockSize; w < (sub_matrix_idx.y + 1) * blockSize; w++) {
            int neighbor = v * width + w;
            if (neighbor >= numOfVertices) continue;

            if (adjMatrix[current_vertex * numOfVertices + neighbor] &&
                atomicCAS(&depth[neighbor], -1, depth[current_vertex] + 1) == -1) {
                int pos = atomicAdd(nextFrontierSize, 1);
                nextFrontier[pos] = neighbor;
            }
        }
    }
}

std::vector<int> GPU_Algorithm::GPU_2DPartition_BFS(const Graph& graph, int src) {
    int numOfVertices = graph.getNumOfVertices();
    int** h_adjMatrix = graph.getAdjacencyMatrix();
    int blockSize = 32;
    int width = (int)sqrt(numOfVertices);

    std::vector<int> _1DAdjMatrix(numOfVertices * numOfVertices);
    std::vector<int2> subMatrixIdx(numOfVertices);
    for (int i = 0; i < numOfVertices; i++) {
        for (int j = 0; j < numOfVertices; j++) {
            _1DAdjMatrix[i * numOfVertices + j] = h_adjMatrix[i][j];
        }
        int row = i / width;
        int col = i % width;
        subMatrixIdx[i] = make_int2(row / blockSize, col / blockSize);
    }

    int* d_adj, * d_depth, * d_currentFrontier, * d_nextFrontier;
    int2* d_subMatrixIdx;
    int* d_currentSize, * d_nextSize;

    cudaMalloc(&d_adj, numOfVertices * numOfVertices * sizeof(int));
    cudaMalloc(&d_depth, numOfVertices * sizeof(int));
    cudaMalloc(&d_subMatrixIdx, numOfVertices * sizeof(int2));
    cudaMalloc(&d_currentFrontier, numOfVertices * sizeof(int));
    cudaMalloc(&d_nextFrontier, numOfVertices * sizeof(int));
    cudaMalloc(&d_currentSize, sizeof(int));
    cudaMalloc(&d_nextSize, sizeof(int));

    std::vector<int> h_depth(numOfVertices, -1);
    h_depth[src] = 0;
    std::vector<int> h_frontier = {src};

    cudaMemcpy(d_adj, _1DAdjMatrix.data(), numOfVertices * numOfVertices * sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(d_depth, h_depth.data(), numOfVertices * sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(d_subMatrixIdx, subMatrixIdx.data(), numOfVertices * sizeof(int2), cudaMemcpyHostToDevice);

    int currentSize = 1;
    int nextSize = 0;

    do {
        cudaMemcpy(d_currentFrontier, h_frontier.data(), currentSize * sizeof(int), cudaMemcpyHostToDevice);
        cudaMemcpy(d_currentSize, &currentSize, sizeof(int), cudaMemcpyHostToDevice);
        cudaMemset(d_nextSize, 0, sizeof(int));

        int gridSize = (currentSize + blockSize - 1) / blockSize;
        bfs2DKernel <<<gridSize, blockSize>>> (d_adj, d_depth, d_subMatrixIdx, numOfVertices, width,
            blockSize, d_currentFrontier, d_currentSize,
            d_nextFrontier, d_nextSize);

        cudaMemcpy(&nextSize, d_nextSize, sizeof(int), cudaMemcpyDeviceToHost);

        std::swap(d_currentFrontier, d_nextFrontier);
        currentSize = nextSize;
        nextSize = 0;

        h_frontier.resize(currentSize);
        cudaMemcpy(h_frontier.data(), d_currentFrontier, currentSize * sizeof(int), cudaMemcpyDeviceToHost);

    } while (currentSize > 0);

    cudaMemcpy(h_depth.data(), d_depth, numOfVertices * sizeof(int), cudaMemcpyDeviceToHost);

    cudaFree(d_adj);
    cudaFree(d_depth);
    cudaFree(d_subMatrixIdx);
    cudaFree(d_currentFrontier);
    cudaFree(d_nextFrontier);
    cudaFree(d_currentSize);
    cudaFree(d_nextSize);

    return h_depth;
}

__device__ int find(int element, int* parent) {
    while (parent[element] != element) {
        int p = parent[element];
        parent[element] = parent[p];
        element = parent[element];
    }
    return element;
}

__device__ bool unite(int setA, int setB, int* parent, int* rank) {
    int rootA = find(setA, parent);
    int rootB = find(setB, parent);
    if (rootA == rootB) return false;

    if (rank[rootA] < rank[rootB]) {
        atomicExch(&parent[rootA], rootB);
    }
    else {
        atomicExch(&parent[rootB], rootA);
        if (rank[rootA] == rank[rootB])
            atomicAdd(&rank[rootA], 1);
    }
    return true;
}

__global__ void filterKruskalKernel(Edge* edges, int numOfEdges, int* parent, int* rank, Edge* mst, int* mstSize, int medianWeight) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx >= numOfEdges) return;

    Edge edge = edges[idx];
    if (edge.weight > medianWeight) return;

    int rootA = find(edge.from, parent);
    int rootB = find(edge.to, parent);

    if (rootA != rootB && unite(rootA, rootB, parent, rank)) {
        int pos = atomicAdd(mstSize, 1);
        mst[pos] = edge;
    }
}

std::vector<GPU_Edge> GPU_Algorithm::GPU_Parallel_FilterKruskal(Graph& graph) {
	std::vector<Edge> edges = graph.getUndirectedEdges();
	std::vector<GPU_Edge> h_edges(edges.size());
    for (auto& edge : edges) {
		h_edges.push_back(GPU_Edge(edge.id, edge.from, edge.to, edge.weight));
    }

	int numOfVertices = graph.getNumOfVertices();
	int numOfEdges = h_edges.size();

    GPU_Edge* d_edges;
    int* d_parent, * d_rank, * d_mstSize;
    GPU_Edge* d_mstEdges;

    cudaMalloc(&d_edges, numOfEdges * sizeof(GPU_Edge));
    cudaMalloc(&d_parent, numOfVertices * sizeof(int));
    cudaMalloc(&d_rank, numOfVertices * sizeof(int));
    cudaMalloc(&d_mstEdges, numOfEdges * sizeof(GPU_Edge));
    cudaMalloc(&d_mstSize, sizeof(int));

    cudaMemcpy(d_edges, h_edges.data(), numOfEdges * sizeof(GPU_Edge), cudaMemcpyHostToDevice);
    cudaMemset(d_rank, 0, numOfVertices * sizeof(int));
    cudaMemset(d_mstSize, 0, sizeof(int));

	thrust::sequence(thrust::device, d_parent, d_parent + numOfVertices);

    thrust::sort(thrust::device, d_edges, d_edges + numOfEdges, [=] __host__ __device__(const GPU_Edge & e1, const GPU_Edge & e2) {
		return e1.weight < e2.weight;
    });

	int remainingEdges = numOfEdges;

    while (remainingEdges > 0) {
        GPU_Edge medianEdge;
        cudaMemcpy(&medianEdge, d_edges + remainingEdges / 2, sizeof(GPU_Edge), cudaMemcpyDeviceToHost);

		int medianWeight = medianEdge.weight;

        int blockSize = 32;
		int gridSize = (remainingEdges + blockSize - 1) / blockSize;

        filterKruskalKernel<<<gridSize, blockSize>>> (d_edges, remainingEdges, d_parent, d_rank, d_mstEdges, d_mstSize, medianWeight);
        cudaDeviceSynchronize();

		auto endOfEdges = thrust::partition(thrust::device, d_edges, d_edges + remainingEdges, [medianWeight] __host__ __device__(const GPU_Edge & e) {
			return e.weight <= medianWeight;
		});
		remainingEdges = (d_edges + remainingEdges) - endOfEdges;
    }

    int finalSize;
    cudaMemcpy(&finalSize, d_mstSize, sizeof(int), cudaMemcpyDeviceToHost);
    std::vector<GPU_Edge> mst(finalSize);
    cudaMemcpy(mst.data(), d_mstEdges, finalSize * sizeof(GPU_Edge), cudaMemcpyDeviceToHost);

    cudaFree(d_edges);
    cudaFree(d_parent);
    cudaFree(d_rank);
    cudaFree(d_mstEdges);
    cudaFree(d_mstSize);

    return mst;
}