#pragma once

#include "record.h"
#include "RecordViewModel.g.h"
#include <winrt/Windows.UI.Xaml.Data.h>

namespace winrt::FYP_V2::implementation
{
    struct RecordViewModel : RecordViewModelT<RecordViewModel>
    {
        RecordViewModel() = default;
        RecordViewModel(int32_t id, hstring exDate, int32_t devIdx, hstring devName, int32_t algIdx, hstring algName, double ts, hstring det) 
            : id(id), executionDate(exDate), deviceIdx(devIdx), deviceName(devName), algorithmIdx(algIdx), algorithmName(algName), timeSpend(ts), details(det) {};

        int32_t Id();
        hstring ExecutionDate();
        int32_t DeviceIdx();
        hstring DeviceName();
        int32_t AlgorithmIdx();
        hstring AlgorithmName();
        double TimeSpend();
        hstring Details();

        //winrt::event_token PropertyChanged(winrt::Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler);
        //void PropertyChanged(winrt::event_token const& token) noexcept;

    private:
        int32_t id;
        hstring executionDate;
        int32_t deviceIdx;
        hstring deviceName;
        int32_t algorithmIdx;
        hstring algorithmName;
        double timeSpend;
        hstring details;

        //winrt::event<winrt::Windows::UI::Xaml::Data::PropertyChangedEventHandler> propertyChanged;
    };
}