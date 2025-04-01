#include "pch.h"
#include "graphManagement.h"
#include <iostream>
#include <unordered_set>

Graph graphManagement::createGraph(std::vector<Node> nodeList, std::unordered_set<Edge> edgeList) {
	Graph graph(nodeList.size());
	for (Edge edge : edgeList) {
		graph.addEdge(edge.from, edge.to, edge.weight);
	}
	return graph;
}