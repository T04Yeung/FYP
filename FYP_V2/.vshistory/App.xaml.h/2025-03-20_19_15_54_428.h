#pragma once

#include "App.xaml.g.h"
#include "record.h"

namespace winrt::FYP_V2::implementation
{
    struct App : AppT<App>
    {
        App();

        FYP_V2::ShareDataViewModel const ShareDataViewModel() { return shareData; }

        void OnLaunched(Microsoft::UI::Xaml::LaunchActivatedEventArgs const&);

    private:
        winrt::Microsoft::UI::Xaml::Window window{ nullptr };
        FYP_V2::ShareDataViewModel shareData{ nullptr };
    };
}
