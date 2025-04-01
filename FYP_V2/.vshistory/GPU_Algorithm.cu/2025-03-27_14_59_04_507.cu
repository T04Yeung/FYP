#include <iostream>
#include <vector>
#include <stack>
#include <unordered_map>
#include <climits>
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

        int blockSize, minGridSize;
        cudaOccupancyMaxPotentialBlockSize(
            &minGridSize,
            &blockSize,
            bfs2DKernel,
            0,
            0
        );

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

std::vector<int> getCompileOrderForKosaraju(const Graph& graph, const int& numOfVertices) {
    std::vector<int> compileOrder;
    std::vector<bool> visited(numOfVertices, false);

    for (int i = 0; i < numOfVertices; i++) {
        if (!visited[i]) {
            std::stack<int> stack;
            stack.push(i);
            visited[i] = true;

            while (!stack.empty()) {
                int currentVertex = stack.top();
                bool hasUnvisitedVertex = false;
                for (int j = 0; j < numOfVertices; j++) {
                    if (graph.getAdjacencyMatrix()[currentVertex][j] != 0 && !visited[j]) {
                        stack.push(j);
                        visited[j] = true;
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

    return compileOrder;
}

__global__ void kosarajuKernel(
    const int* adjMatrix,
    const int* compileOrder,
    bool* visited,
    int* components,
    int numVertices
) {
    int idx = numVertices - 1 - (blockIdx.x * blockDim.x + threadIdx.x);
    if (idx < 0) return;

    int currentVertex = compileOrder[idx];

    bool expected = false;
    /*if (atomicCAS(&visited[currentVertex], expected, true)) {
        int componentId = currentVertex;
        components[currentVertex] = componentId;

        int* queue = new int[numVertices];
        int front = 0, rear = 0;
        queue[rear++] = currentVertex;

        while (front < rear) {
            int currentV = queue[front++];
            for (int i = 0; i < numVertices; i++) {
                if (adjMatrix[currentV * numVertices + i] && !visited[i]) {
                    if (atomicCAS(&visited[i], false, true)) {
                        components[i] = componentId;
                        queue[rear++] = i;
                    }
                }
            }
        }
        delete[] queue;
    }*/
}

std::vector<std::vector<int>> GPU_Algorithm::GPU_Kosaraju(Graph& graph) {
	int numOfVertices = graph.getNumOfVertices();
	std::vector<int> compileOrder = getCompileOrderForKosaraju(graph, numOfVertices);

	Graph reverseGraph = graph.getReversedGraph();
    int** reversedAdjMatrix = reverseGraph.getAdjacencyMatrix();

    std::vector<int> reversed1DAdj(numOfVertices * numOfVertices);
    for (int i = 0; i < numOfVertices; ++i) {
        for (int j = 0; j < numOfVertices; ++j) {
            reversed1DAdj[i * numOfVertices + j] = reversedAdjMatrix[i][j];
        }
    }

    int* d_adj, * d_compileOrder, * d_components;
    bool* d_visited;
    cudaMalloc(&d_adj, numOfVertices * numOfVertices * sizeof(int));
    cudaMalloc(&d_compileOrder, numOfVertices * sizeof(int));
    cudaMalloc(&d_components, numOfVertices * sizeof(int));
    cudaMalloc(&d_visited, numOfVertices * sizeof(bool));

    cudaMemcpy(d_adj, reversed1DAdj.data(), numOfVertices * numOfVertices * sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(d_compileOrder, compileOrder.data(), numOfVertices * sizeof(int), cudaMemcpyHostToDevice);
    cudaMemset(d_components, -1, numOfVertices * sizeof(int));
    cudaMemset(d_visited, false, numOfVertices * sizeof(bool));

    int blockSize, minGridSize;
    cudaOccupancyMaxPotentialBlockSize(
        &minGridSize,
        &blockSize,
        dijkstraKernel,
        0,
        0
    );

    int gridSize = (numOfVertices + blockSize - 1) / blockSize;

    kosarajuKernel <<<gridSize, blockSize >>> (d_adj, d_compileOrder, d_visited, d_components, numOfVertices);
    cudaDeviceSynchronize();

    std::vector<int> h_components(numOfVertices);
    cudaMemcpy(h_components.data(), d_components, numOfVertices * sizeof(int), cudaMemcpyDeviceToHost);

    std::unordered_map<int, std::vector<int>> componentMap;
    for (int i = 0; i < numOfVertices; ++i) {
        componentMap[h_components[i]].push_back(i);
    }

    cudaFree(d_adj);
    cudaFree(d_compileOrder);
    cudaFree(d_components);
    cudaFree(d_visited);

    std::vector<std::vector<int>> sccs;
    for (const auto& pair : componentMap) {
        sccs.push_back(pair.second);
    }

    return sccs;
}