#pragma once

#include "pch.h"
#include "graph.h"
#include "graphManagement.h"
#include "fileManagement.h"
#include <vector>
#include <unordered_map>
#include <unordered_set>

class SharedData {
public:
    static SharedData& getInstance() {
        static SharedData instance;
        return instance;
    }

    Graph graph;
    std::vector<Node> currentNodes;
    std::vector<Edge> currentEdges;

    int addSnapshot(const std::unordered_set<Edge>& edges) {
        int id = nextSnapshotId++;
        snapshotEdges[id] = edges;
        for (const auto& edge : edges) {
            edgeCounts[edge]++;
        }
        totalSnapshots++;
        return id;
    }

    void removeSnapshot(int id) {
        if (!snapshotEdges.count(id)) return;
        for (const auto& edge : snapshotEdges[id]) {
            if (--edgeCounts[edge] == 0) {
                edgeCounts.erase(edge);
            }
        }
        snapshotEdges.erase(id);
        totalSnapshots--;
    }

    std::unordered_set<Edge> getCommonEdge() {
        std::unordered_set<Edge> commonEdges;
        for (const auto& [edge, count] : edgeCounts) {
            if (count == totalSnapshots) {
                commonEdges.insert(edge);
            }
        }
        return commonEdges;
    }

    std::unordered_set<Edge> getSnapshotDelta(int id) {
        auto commonEdges = getCommonEdge();
        std::unordered_set<Edge> delta;
        for (const auto& edge : snapshotEdges[id]) {
            if (!commonEdges.count(edge)) {
                delta.insert(edge);
            }
        }
        return delta;
    }

private:
    SharedData() = default;
    ~SharedData() = default;
    SharedData(const SharedData&) = delete;
    SharedData& operator=(const SharedData&) = delete;

    std::map<Edge, int> edgeCounts;
    std::unordered_map<int, std::unordered_set<Edge>> snapshotEdges;
    int totalSnapshots = 0;
    int nextSnapshotId = 0;

};