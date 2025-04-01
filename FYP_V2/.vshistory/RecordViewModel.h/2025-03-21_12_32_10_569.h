﻿#pragma once

#include "record.h"
#include "RecordViewModel.g.h"
#include <winrt/Windows.UI.Xaml.Data.h>

namespace winrt::FYP_V2::implementation
{
    struct RecordViewModel : RecordViewModelT<RecordViewModel>
    {
        RecordViewModel::RecordViewModel(Record& record)
        {
            id = record.getId();
            executionDate = to_hstring(record.getExecutionDateStr());
            deviceIdx = record.getDeviceIdx();
            deviceName = to_hstring(record.getDeviceName());
            algorithmIdx = record.getAlgorithmIdx();
            algorithmName = to_hstring(record.getAlgorithmName());
            timeSpend = record.getTimeSpend();
            //details = to_hstring(record.getDetails().dump());
        }

        int32_t getId();
        hstring getExecutionDate();
        int32_t getDeviceIdx();
        hstring getDeviceName();
        int32_t getAlgorithmIdx();
        hstring getAlgorithmName();
        double getTimeSpend();
        //hstring getDetails();

        winrt::event_token PropertyChanged(winrt::Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler);
        void PropertyChanged(winrt::event_token const& token) noexcept;

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