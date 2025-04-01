#pragma once

#include <iostream>
#include <vector>
#include "graph.h"
#include "dataStructure.h"

class CPU_Algorithm {
public:
	static std::vector<std::vector<int>> CPU_Parallel_Dijkstra(const Graph& graph);
	static std::vector<std::vector<int>> CPU_2DBlockMappedFW(const Graph& graph);
	static std::vector<int> CPU_2DPartition_BFS(const Graph& graph, int src);
	static std::vector<Edge> CPU_Parallel_FilterKruskal(Graph& graph);
	static int CPU_Triangle_Counting(Graph& graph);
};