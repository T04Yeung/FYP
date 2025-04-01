#pragma once

#include "RecordViewModel.g.h"

namespace winrt::FYP_V2::implementation
{
    struct RecordViewModel : RecordViewModelT<RecordViewModel>
    {
        int32_t MyProperty();
        void MyProperty(int32_t value);
    };
}

namespace winrt::FYP_V2::factory_implementation
{
    struct RecordViewModel : RecordViewModelT<RecordViewModel, implementation::RecordViewModel>
    {
    };
}
