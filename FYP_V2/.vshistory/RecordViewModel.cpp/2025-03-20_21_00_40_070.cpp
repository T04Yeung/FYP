#include "pch.h"
#include "RecordViewModel.h"
#include "record.h"
#if __has_include("RecordViewModel.g.cpp")
#include "RecordViewModel.g.cpp"
#endif

namespace winrt::FYP_V2::implementation
{
    int32_t RecordViewModel::getId() {
        return id;
    }

    hstring RecordViewModel::getExecutionDate() {
        return executionDate;
    }

    int32_t RecordViewModel::getDeviceIdx() {
        return deviceIdx;
    }

    hstring RecordViewModel::getDeviceName() {
        return deviceName;
    }

    int32_t RecordViewModel::getAlgorithmIdx() {
        return algorithmIdx;
    }

    hstring RecordViewModel::getAlgorithmName() {
        return algorithmName;
    }

    double RecordViewModel::getTimeSpend() {
        return timeSpend;
    }

    hstring RecordViewModel::getDetails() {
        return details;
    }
}
