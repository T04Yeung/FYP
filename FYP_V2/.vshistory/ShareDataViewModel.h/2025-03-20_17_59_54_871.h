#pragma once

#include "ShareDataViewModel.g.h"

namespace winrt::FYP_V2::implementation
{
    struct ShareDataViewModel : ShareDataViewModelT<ShareDataViewModel>
    {
        ShareDataViewModel() = default;

    };
}

namespace winrt::FYP_V2::factory_implementation
{
    struct ShareDataViewModel : ShareDataViewModelT<ShareDataViewModel, implementation::ShareDataViewModel>
    {
    };
}
