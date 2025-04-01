#pragma once

#include "App.xaml.g.h"
#include "record.h"

namespace winrt::FYP_V2::implementation
{
    struct App : AppT<App>
    {
        App();

        static FYP_V2::ShareDataViewModel ShareDataViewModel();

        void OnLaunched(Microsoft::UI::Xaml::LaunchActivatedEventArgs const&);

    private:
        winrt::Microsoft::UI::Xaml::Window window{ nullptr };
        static winrt::FYP_V2::ShareDataViewModel shareDataViewModel;
    };
}
