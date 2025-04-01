#pragma once

#include "MainWindow.g.h"

namespace winrt::FYP_V2::implementation
{
    struct MainWindow : MainWindowT<MainWindow>
    {
        MainWindow();
        void Nav_ItemInvoked(const NavigationViewItemInvokedEventArgs& args);
        int32_t MyProperty();
        void MyProperty(int32_t value);
    };
}

namespace winrt::FYP_V2::factory_implementation
{
    struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
    {
    };
}
