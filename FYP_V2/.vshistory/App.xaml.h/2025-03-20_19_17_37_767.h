#pragma once

#include "App.xaml.g.h"
#include "record.h"

namespace winrt::FYP_V2::implementation
{
    struct App : AppT<App>
    {
        App();

        static FYP_V2::ShareDataViewModel ShareDataViewModel() { return shareData; }

        void OnLaunched(Microsoft::UI::Xaml::LaunchActivatedEventArgs const&);

    private:
        winrt::Microsoft::UI::Xaml::Window window{ nullptr };
        static FYP_V2::ShareDataViewModel shareData;
    };
}
