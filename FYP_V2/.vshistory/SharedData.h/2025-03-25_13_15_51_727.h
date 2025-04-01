﻿#pragma once

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

    std::vector<Node> currentNodes;
    std::unordered_set<Edge> currentEdges;

    void addSnapshot() {
		int currentId = nextSnapshotId++;
		
        for (const auto& edge : currentEdges) {
			edgeCounts[edge]++;
        }
        totalSnapshots++;

        rebuildGraph(currentId);
    }

    void rebuildGraph(int id) {
		for (const auto& [edge, count] : edgeCounts) {
			if (count == totalSnapshots) {
				commonEdges.insert(edge);
			}
			else {
                deltas[id].insert(edge);
			}
		}
    }

private:
    SharedData() = default;
    ~SharedData() = default;
    SharedData(const SharedData&) = delete;
    SharedData& operator=(const SharedData&) = delete;

    std::map<Edge, int> edgeCounts;
	std::unordered_set<Edge> commonEdges;
    std::unordered_map<int, std::unordered_set<Edge>> deltas;
    int totalSnapshots = 0;
    int nextSnapshotId = 0;

};