#pragma once

#include "HomePage.g.h"
#include "graph.h"
#include "fileManagement.h"
#include <vector>

namespace winrt::FYP_V2::implementation
{
    struct HomePage : HomePageT<HomePage>
    {
        std::vector<Graph> graphList;
		std::vector<Node> currentNodes;
		std::vector<Edge> currentEdges;
        int currentGraph = 0;
        HomePage()
        {
            InitializeComponent();
            MenuBarContainer().Translation({ 0, 0, 32 });
            LeftPanel().Translation({ 0, 0, 32 });
            RightPanel().Translation({ 0, 0, 32 });
        }

        int32_t MyProperty();
        void MyProperty(int32_t value);
        void ImportVertices(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void ImportEdges(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);

        winrt::fire_and_forget ShowMessage(hstring message);
        void executionBtn_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
    };
}

namespace winrt::FYP_V2::factory_implementation
{
    struct HomePage : HomePageT<HomePage, implementation::HomePage>
    {
    };
}
