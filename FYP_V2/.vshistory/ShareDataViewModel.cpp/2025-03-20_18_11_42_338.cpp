#include "pch.h"
#include "ShareDataViewModel.h"
#if __has_include("ShareDataViewModel.g.cpp")
#include "ShareDataViewModel.g.cpp"
#endif

namespace winrt::FYP_V2::implementation
{
    void ShareDataViewModel::addingGraph(Graph graph) {
        graphList.emplace_back(graph);
    }

    Graph ShareDataViewModel::getFirstGraph() {
        return graphList.front();
    }

    std::vector<Node> ShareDataViewModel::getCurrentNodes() {
        return currentNodes;
    }

    std::vector<Edge> ShareDataViewModel::getCurrentEdges() {
        return currentEdges;
    }

    void ShareDataViewModel::setCurrentNodes(std::vector<Node> nodeList) {
        currentNodes = nodeList;
    }

    void ShareDataViewModel::setCurrentEdges(std::vector<Edge> edgeList) {
        currentEdges = edgeList;
    }
}
