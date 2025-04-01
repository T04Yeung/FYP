#pragma once

#include "graph.h"
#include "fileManagement.h"
#include <vector>

class SharedState {
public:
    static SharedState& getInstance() {
        static SharedState instance;
        return instance;
    }

    Graph graph;
    std::vector<Node> currentNodes;
    std::vector<Edge> currentEdges;

private:
    SharedState() = default;
    ~SharedState() = default;
    SharedState(const SharedState&) = delete;
    SharedState& operator=(const SharedState&) = delete;
};