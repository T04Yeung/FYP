#include "pch.h"
#include "RecordViewModel.h"
#include "record.h"
#if __has_include("RecordViewModel.g.cpp")
#include "RecordViewModel.g.cpp"
#endif

namespace winrt::FYP_V2::implementation
{
    winrt::FYP_V2::RecordViewModel RecordViewModel::Create(
        int32_t id,
        winrt::hstring executionDate,
        int32_t deviceIdx,
        winrt::hstring deviceName,
        int32_t algorithmIdx,
        winrt::hstring algorithmName,
        double timeSpend,
        winrt::hstring details
    ) {
        auto instance = winrt::make_self<RecordViewModel>();
        instance->id = id;
        instance->executionDate = executionDate;
        instance->deviceIdx = deviceIdx;
        instance->deviceName = deviceName;
        instance->algorithmIdx = algorithmIdx;
        instance->algorithmName = algorithmName;
        instance->timeSpend = timeSpend;
        instance->details = details;
        return *instance;
    }

    int32_t RecordViewModel::Id() {
        return id;
    }

    hstring RecordViewModel::ExecutionDate() {
        return executionDate;
    }

    int32_t RecordViewModel::DeviceIdx() {
        return deviceIdx;
    }

    hstring RecordViewModel::DeviceName() {
        return deviceName;
    }

    int32_t RecordViewModel::AlgorithmIdx() {
        return algorithmIdx;
    }

    hstring RecordViewModel::AlgorithmName() {
        return algorithmName;
    }

    double RecordViewModel::TimeSpend() {
        return timeSpend;
    }

    hstring RecordViewModel::Details() {
        return details;
    }
}
