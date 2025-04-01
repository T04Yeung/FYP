#pragma once

#include "ShareDataViewModel.g.h"
#include "graph.h"
#include "fileManagement.h"

namespace winrt::FYP_V2::implementation
{
    struct ShareDataViewModel : ShareDataViewModelT<ShareDataViewModel>
    {
        ShareDataViewModel() = default;

        void addingGraph(Graph graph);
        Graph getFirstGraph();
        std::vector<Node> getCurrentNodes();
        std::vector<Edge> getCurrentEdges();
        void setCurrentNodes(std::vector<Node> nodeList);
        void setCurrentEdges(std::vector<Edge> edgeList);

    private:
        std::vector<Graph> graphList;
        std::vector<Node> currentNodes;
        std::vector<Edge> currentEdges;
        int currentGraphIdx = 0;
    };
}

namespace winrt::FYP_V2::factory_implementation
{
    struct ShareDataViewModel : ShareDataViewModelT<ShareDataViewModel, implementation::ShareDataViewModel>
    {
    };
}
