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
            RecordListContainer().Translation({ 0, 0, 32 });

            displayRecords = winrt::single_threaded_observable_vector<FYP_V2::RecordViewModel>();

            std::vector<Record> records = fileFunctions::getAllRecords();

            for (auto& record : records) {
                displayRecords.Append(RecordViewModel::Create(
                    record.getId(),
                    winrt::to_hstring(record.getExecutionDateStr()),
                    record.getDeviceIdx(),
                    winrt::to_hstring(record.getDeviceName()),
                    record.getAlgorithmIdx(),
                    winrt::to_hstring(record.getAlgorithmName()),
                    record.getTimeSpend(),
                    winrt::to_hstring(record.getDetails().dump())
                ));
            }
        }

        Windows::Foundation::Collections::IObservableVector<FYP_V2::RecordViewModel> Records() const;

    private:
        Windows::Foundation::Collections::IObservableVector<FYP_V2::RecordViewModel> displayRecords;
    };
}

namespace winrt::FYP_V2::factory_implementation
{
    struct RecordPage : RecordPageT<RecordPage, implementation::RecordPage>
    {
    };
}
