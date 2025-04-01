#pragma once

#include "pch.h"
#include "graph.h"
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

    std::vector<std::vector<int>> edgeCount;
    Graph commonGraph;
    std::unordered_map<int, std::vector<Edge>> deltas;
    int totalSnapshots = 0;
    int currentSnapshotIdx = 0;

private:
    SharedData() = default;
    ~SharedData() = default;
    SharedData(const SharedData&) = delete;
    SharedData& operator=(const SharedData&) = delete;


};