#pragma once

#include "pch.h"
#include "graph.h"
#include "graphManagement.h"
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
    std::unordered_set<Edge> currentEdges;
    std::unordered_map<int, std::unordered_set<Edge>> snapshotsEdges;

    int totalSnapshots = 0;
    int currentDisplayGraphId = -1;
    std::vector<std::pair<double, double>> nodePositions;

    void addSnapshot() {
        int currentId = nextSnapshotId++;
        currentDisplayGraphId = currentId;

        snapshotsEdges[currentId] = currentEdges;

        for (const auto& edge : currentEdges) {
            edgeCounts[edge]++;
        }
        totalSnapshots++;

        rebuildCommonGraph();
        computeAllDeltas();
    }

    Graph getGraph(int id) {
        std::unordered_set<Edge> fullEdges = commonEdges;
		fullEdges.insert(deltas[id].begin(), deltas[id].end());

		return graphManagement::createGraph(currentNodes, fullEdges);
    }

    void removeSnapshot(int id) {
        if (!snapshotsEdges.count(id)) return;

        for (const auto& edge : snapshotsEdges[id]) {
            if (--edgeCounts[edge] == 0) {
                edgeCounts.erase(edge);
            }
        }

		auto iter = snapshotsEdges.find(id);
        if (++iter != snapshotsEdges.end()) {
			currentDisplayGraphId = iter->first;
		}
		else {
            iter--; iter--;
			currentDisplayGraphId = iter->first;
        }

        snapshotsEdges.erase(id);
        deltas.erase(id);
        totalSnapshots--;

        rebuildCommonGraph();
        computeAllDeltas();
    }

private:
    SharedData() = default;
    ~SharedData() = default;
    SharedData(const SharedData&) = delete;
    SharedData& operator=(const SharedData&) = delete;

    std::map<Edge, int> edgeCounts;
    std::unordered_set<Edge> commonEdges;
    std::unordered_map<int, std::unordered_set<Edge>> deltas;
    int nextSnapshotId = 0;

    void rebuildCommonGraph() {
        commonEdges.clear();
        if (snapshotsEdges.empty()) return;

        auto common = snapshotsEdges.begin()->second;
        for (const auto& [id, edges] : snapshotsEdges) {
            std::unordered_set<Edge> temp;
            for (const auto& edge : edges) {
                if (common.count(edge)) temp.insert(edge);
            }
            common = temp;
        }
        commonEdges = common;
    }

    void computeAllDeltas() {
        deltas.clear();
        for (const auto& [id, edges] : snapshotsEdges) {
            std::unordered_set<Edge> delta;
            for (const auto& edge : edges) {
                if (!commonEdges.count(edge)) {
                    delta.insert(edge);
                }
            }
            deltas[id] = delta;
        }
    }
};