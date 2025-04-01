#pragma once

#include "record.h"
#include "RecordViewModel.g.h"
#include <winrt/Windows.UI.Xaml.Data.h>

namespace winrt::FYP_V2::implementation
{
    struct RecordViewModel : RecordViewModelT<RecordViewModel>
    {
        RecordViewModel(Record record) {
            id = record.getId();
            executionDate = winrt::to_hstring(record.getExecutionDateStr());
            deviceIdx = record.getDeviceIdx();
            deviceName = winrt::to_hstring(record.getDeviceName());
            algorithmIdx = record.getAlgorithmIdx();
            algorithmName = winrt::to_hstring(record.getAlgorithmName());
            timeSpend = record.getTimeSpend();
            details = winrt::to_hstring(record.getDetails());
        }

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

        winrt::event<winrt::Windows::UI::Xaml::Data::PropertyChangedEventHandler> propertyChanged;
    };
}