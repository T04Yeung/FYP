#pragma once

#include "HomePage.g.h"

namespace winrt::FYP_V2::implementation
{
    struct HomePage : HomePageT<HomePage>
    {
        HomePage()
        {
            InitializeComponent();
            MenuBarContainer().Translation({ 0, 0, 32 });
            LeftPanel().Translation({ 0, 0, 32 });
            RightPanel().Translation({ 0, 0, 32 });
        }

        int32_t MyProperty();
        void MyProperty(int32_t value);
        void OpenFileDialog(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
    };
}

namespace winrt::FYP_V2::factory_implementation
{
    struct HomePage : HomePageT<HomePage, implementation::HomePage>
    {
    };
}
