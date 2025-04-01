#pragma once

#include "MainWindow.g.h"

namespace winrt::FYP_V2::implementation
{
    struct MainWindow : MainWindowT<MainWindow>
    {
        MainWindow();
        int32_t MyProperty();
        void MyProperty(int32_t value);
        void Nav_SelectionChanged(winrt::Microsoft::UI::Xaml::Controls::NavigationView const& sender, winrt::Microsoft::UI::Xaml::Controls::NavigationViewSelectionChangedEventArgs const& args);

        std::vector<Graph> graphList;
        std::vector<Node> currentNodes;
        std::vector<Edge> currentEdges;
        int currentGraphIdx = 0;
    };
}

namespace winrt::FYP_V2::factory_implementation
{
    struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
    {
    };
}
