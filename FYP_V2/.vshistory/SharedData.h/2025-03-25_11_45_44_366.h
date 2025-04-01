#pragma once

#include "pch.h"
#include "graph.h"
#include "graphManagement.h"
#include "fileManagement.h"
#include <vector>
#include <unordered_map>

class SharedData {
public:
    static SharedData& getInstance() {
        static SharedData instance;
        return instance;
    }

    Graph graph;
    std::vector<Node> currentNodes;
    std::vector<Edge> currentEdges;

    void addSnapshot() {
		totalSnapshots++;

        currentSnapshotIdx = currentSnapshotIdx + 1 % 9;

        for (auto& edge: currentEdges) {
			edgeCount[edge.from][edge.to]++;
            if (edgeCount[edge.from][edge.to] == totalSnapshots) {
				commonGraph.addEdge(edge.from, edge.to, 1);
            }
        }
    }

    Graph getCurrenGraph() {
		Graph currentGraph = commonGraph;
        for (Edge& edge : deltas[currentSnapshotIdx]) {
			currentGraph.addEdge(edge.from, edge.to, edge.weight);
        }
    }

private:
    SharedData() = default;
    ~SharedData() = default;
    SharedData(const SharedData&) = delete;
    SharedData& operator=(const SharedData&) = delete;

    std::vector<std::vector<int>> edgeCount;
    Graph commonGraph;
    std::unordered_map<int, std::vector<Edge>> deltas;
    int totalSnapshots = 0;
    int currentSnapshotIdx = -1;
};