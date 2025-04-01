#pragma once

#include "App.xaml.g.h"
#include "record.h"
#include <vector>

namespace winrt::FYP_V2::implementation
{
    struct App : AppT<App>
    {
        App();

        FYP_V2::ShareDataViewModel ShareDataViewModel();

        void OnLaunched(Microsoft::UI::Xaml::LaunchActivatedEventArgs const&);

    private:
        winrt::Microsoft::UI::Xaml::Window window{ nullptr };
        winrt::FYP_V2::ShareDataViewModel shareDataViewModel;
    };
}
