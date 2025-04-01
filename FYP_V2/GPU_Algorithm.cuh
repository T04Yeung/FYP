#pragma once

#include "graph.h"
#include "dataStructure.h"
#include <vector>

class GPU_Algorithm {
public:
    static std::vector<std::vector<int>> GPU_Parallel_Dijkstra(const Graph& graph);
    static std::vector<std::vector<int>> GPU_2DBlockMappedFW(const Graph& graph);
    static std::vector<int> GPU_2DPartition_BFS(const Graph& graph, int src);
	static std::vector<Edge> GPU_Parallel_FilterKruskal(Graph& graph);
    static int GPU_Triangle_Counting(Graph& graph);
};