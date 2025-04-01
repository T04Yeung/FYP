#pragma once

#include "graph.h"
#include "dataStructure.h"
#include <vector>

struct GPU_Edge {
    int id;
    int from, to, weight;

public:
    __host__ __device__ GPU_Edge() : id(-1), from(-1), to(-1), weight(-1) {}

    __host__ __device__ GPU_Edge(int id, int from, int to, int weight)
        : id(id), from(from), to(to), weight(weight) {
    }

    __host__ __device__ GPU_Edge(const GPU_Edge& other)
        : id(other.id), from(other.from), to(other.to), weight(other.weight) {
    }

    __host__ __device__ bool operator==(const GPU_Edge& other) const {
        return id == other.id && from == other.from && to == other.to && weight == other.weight;
    }

    __host__ __device__ bool operator<(const GPU_Edge& other) const {
        return weight < other.weight;
    }
};

class GPU_Algorithm {
public:
    static std::vector<std::vector<int>> GPU_Parallel_Dijkstra(const Graph& graph);
    static std::vector<std::vector<int>> GPU_2DBlockMappedFW(const Graph& graph);
    static std::vector<int> GPU_2DPartition_BFS(const Graph& graph, int src);
	static std::vector<GPU_Edge> GPU_Parallel_FilterKruskal(Graph& graph);
};