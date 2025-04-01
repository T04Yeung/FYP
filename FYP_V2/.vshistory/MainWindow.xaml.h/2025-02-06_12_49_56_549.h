#pragma once

#include "MainWindow.g.h"

namespace winrt::FYP_V2::implementation
{
    struct MainWindow : MainWindowT<MainWindow>
    {
        MainWindow();
        void MainWindow::Nav_ItemInvoked(const NavigationView&, const NavigationViewItemInvokedEventArgs& args);
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
