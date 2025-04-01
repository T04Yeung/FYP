#pragma once

#include "RecordPage.g.h"
#include "fileManagement.h"
#include "record.h"
#include <vector>
#include <winrt/Windows.Foundation.Collections.h>

namespace winrt::FYP_V2::implementation
{
    struct RecordPage : RecordPageT<RecordPage>
    {
        RecordPage()
        {
            InitializeComponent();
            SearchBarContainer().Translation({0, 0, 32});

            std::vector<Record> records = fileFunctions::getAllRecords();

            for (auto& record : records) {
                FYP_V2::RecordViewModel recordVM = winrt::make<FYP_V2::RecordViewModel>(record);
            }
        }

        Windows::Foundation::Collections::IObservableVector<FYP_V2::RecordViewModel> Records() const;

    private:
        Windows::Foundation::Collections::IObservableVector<FYP_V2::RecordViewModel> displayRecords{
            winrt::single_threaded_observable_vector<FYP_V2::RecordViewModel>()
        };
    };
}

namespace winrt::FYP_V2::factory_implementation
{
    struct RecordPage : RecordPageT<RecordPage, implementation::RecordPage>
    {
    };
}
