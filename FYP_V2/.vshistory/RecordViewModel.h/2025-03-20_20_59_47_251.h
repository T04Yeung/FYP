﻿#pragma once

#include "RecordViewModel.g.h"
#include <winrt/Windows.UI.Xaml.Data.h>

namespace winrt::FYP_V2::implementation
{
    struct RecordViewModel : RecordViewModelT<RecordViewModel>
    {
        RecordViewModel(Record& record);

        int32_t getId();
        hstring getExecutionDate();
        int32_t getDeviceIdx();
        hstring getDeviceName();
        int32_t getAlgorithmIdx();
        hstring getAlgorithmName();
        double getTimeSpend();
        hstring getDetails();

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