﻿#pragma once
#include "RecordViewModel.g.h"
#include "dataStructure.h"

namespace winrt::FYP_V2::implementation
{
    struct RecordViewModel : RecordViewModelT<RecordViewModel>
    {
        static winrt::FYP_V2::RecordViewModel Create(
            int32_t id,
            winrt::hstring executionDate,
            int32_t deviceIdx,
            winrt::hstring deviceName,
            int32_t algorithmIdx,
            winrt::hstring algorithmName,
            double timeSpend,
            std::vector<Node> nodeList,
            winrt::hstring details
        );

        int32_t Id();
        winrt::hstring ExecutionDate();
        int32_t DeviceIdx();
        winrt::hstring DeviceName();
        int32_t AlgorithmIdx();
        winrt::hstring AlgorithmName();
        double TimeSpend();
        std::vector<winrt::hstring> NodeList();
        winrt::hstring Details();

    private:
        int32_t id;
        winrt::hstring executionDate;
        int32_t deviceIdx;
        winrt::hstring deviceName;
        int32_t algorithmIdx;
        winrt::hstring algorithmName;
        double timeSpend;
        std::vector<winrt::hstring> nodeList;
        winrt::hstring details;
    };
}

namespace winrt::FYP_V2::factory_implementation
{
    struct RecordViewModel : RecordViewModelT<RecordViewModel, implementation::RecordViewModel>
    {
    };
}
