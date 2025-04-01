#include "pch.h"
#include "RecordViewModel.h"
#include "record.h"
#if __has_include("RecordViewModel.g.cpp")
#include "RecordViewModel.g.cpp"
#endif

namespace winrt::FYP_V2::implementation
{
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

    winrt::event_token RecordViewModel::PropertyChanged(winrt::Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler) {
        return propertyChanged.add(handler);
    }

    void RecordViewModel::PropertyChanged(winrt::event_token const& token) noexcept {
        propertyChanged.remove(token);
    }
}
