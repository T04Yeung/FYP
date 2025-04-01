#pragma once

#include "record.h"
#include "RecordViewModel.g.h"
#include <winrt/Windows.UI.Xaml.Data.h>

namespace winrt::FYP_V2::implementation
{
    struct RecordViewModel : RecordViewModelT<RecordViewModel>
    {
        RecordViewModel() = default;

        static winrt::FYP_V2::RecordViewModel Create(
            int32_t id,
            hstring executionDate,
            int32_t deviceIdx,
            hstring deviceName,
            int32_t algorithmIdx,
            hstring algorithmName,
            double timeSpend,
            hstring details
        );

        int32_t Id();
        hstring ExecutionDate();
        int32_t DeviceIdx();
        hstring DeviceName();
        int32_t AlgorithmIdx();
        hstring AlgorithmName();
        double TimeSpend();
        hstring Details();

    private:
        int32_t id;
        hstring executionDate;
        int32_t deviceIdx;
        hstring deviceName;
        int32_t algorithmIdx;
        hstring algorithmName;
        double timeSpend;
        hstring details;
    };
}