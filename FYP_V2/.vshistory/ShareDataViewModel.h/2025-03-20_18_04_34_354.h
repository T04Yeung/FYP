#pragma once

#include "ShareDataViewModel.g.h"
#include "graph.h"
#include "fileManagement.h"

namespace winrt::FYP_V2::implementation
{
    struct ShareDataViewModel : ShareDataViewModelT<ShareDataViewModel>
    {
        ShareDataViewModel() = default;

        std::vector<Graph> graphList;
        std::vector<Node> currentNodes;
        std::vector<Edge> currentEdges;
        int currentGraphIdx;
    };
}

namespace winrt::FYP_V2::factory_implementation
{
    struct ShareDataViewModel : ShareDataViewModelT<ShareDataViewModel, implementation::ShareDataViewModel>
    {
    };
}
