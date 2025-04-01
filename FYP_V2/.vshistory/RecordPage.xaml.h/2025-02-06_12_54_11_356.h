#pragma once

#include "RecordPage.g.h"

namespace winrt::FYP_V2::implementation
{
    struct RecordPage : RecordPageT<RecordPage>
    {
        RecordPage()
        {
            // Xaml objects should not call InitializeComponent during construction.
            // See https://github.com/microsoft/cppwinrt/tree/master/nuget#initializecomponent
        }

        int32_t MyProperty();
        void MyProperty(int32_t value);
    };
}

namespace winrt::FYP_V2::factory_implementation
{
    struct RecordPage : RecordPageT<RecordPage, implementation::RecordPage>
    {
    };
}
