#include "pch.h"
#include "graphManagement.h"
#include <iostream>

Graph graphManagement::createGraph(std::vector<Node> nodeList, std::vector<Edge> edgeList) {
	Graph graph(nodeList.size());
	for (Edge edge : edgeList) {
		graph.addEdge(edge.from, edge.to, edge.weight);
	}
	return graph;
}
