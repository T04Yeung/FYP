#pragma once

#include "RecordPage.g.h"
#include <winrt/Windows.Foundation.Collections.h>

namespace winrt::FYP_V2::implementation
{
    struct RecordPage : RecordPageT<RecordPage>
    {
        RecordPage()
        {
            InitializeComponent();
            SearchBarContainer().Translation({0, 0, 32});
        }

        Windows::Foundation::Collections::IObservableVector<FYP_V2::RecordViewModel> Records() const;

    private:
        Windows::Foundation::Collections::IObservableVector<FYP_V2::RecordViewModel> records{
            winrt::single_threaded_observable_vector<FYP_V2::RecordViewModel>()
        };

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
