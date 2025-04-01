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
        HomePage()
        {
            InitializeComponent();
            MenuBarContainer().Translation({ 0, 0, 32 });
            LeftPanel().Translation({ 0, 0, 32 });
            RightPanel().Translation({ 0, 0, 32 });
        }

        int32_t MyProperty();
		void ShowMessage(std::string message);
        void MyProperty(int32_t value);
        void ImportVertices(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void ImportEdges(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
    };
}

namespace winrt::FYP_V2::factory_implementation
{
    struct HomePage : HomePageT<HomePage, implementation::HomePage>
    {
    };
}
