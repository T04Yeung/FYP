#pragma once

#include <vector>
#include "graph.h"
#include "fileManagement.h"

class graphManagement {
public:
	static Graph createGraph(std::vector<Node> nodeList, std::vector<Edge> edgeList);
	static void displayGraph(Graph graph);
};