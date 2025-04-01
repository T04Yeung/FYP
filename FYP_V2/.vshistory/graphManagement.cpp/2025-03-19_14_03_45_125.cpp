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

std::string graphManagement::displayGraph(Graph graph) {
	int numOfVertices = graph.getNumOfVertices();
	int** adjacencyMatrix = graph.getAdjacencyMatrix();

	for (int i = 0; i < numOfVertices; i++) {
		for (int j = 0; j < numOfVertices; j++) {
			std::cout << adjacencyMatrix[i][j] << " ";
		}
		std::cout << std::endl;
	}
}