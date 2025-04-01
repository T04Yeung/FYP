#pragma once

#include <iostream>
#include <vector>
#include "graph.h"

class CPU_Algorithm {
public:
	static std::vector<std::vector<int>> CPU_Parallel_Dijkstra(const Graph& graph);
	static std::vector<int> CPU_Algorithm::CPU_2DPartition_BFS(const Graph& graph, int src);
	std::vector<std::vector<int>> CPU_Algorithm::CPU_Kosaraju(Graph& graph);

};