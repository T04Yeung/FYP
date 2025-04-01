#pragma once

#include "pch.h"
#include "graph.h"
#include "fileManagement.h"
#include <vector>

class SharedData {
public:
    static SharedData& getInstance() {
        static SharedData instance;
        return instance;
    }

    Graph graph;
    std::vector<Node> currentNodes;
    std::vector<Edge> currentEdges;

    float graphZoomFactor = 0.1f;
    double graphScrollHorizontal = 0.0;
    double graphScrollVertical = 0.0;

private:
    SharedData() = default;
    ~SharedData() = default;
    SharedData(const SharedData&) = delete;
    SharedData& operator=(const SharedData&) = delete;
};