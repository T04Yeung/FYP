#pragma once

#include "RecordPage.g.h"
#include "fileManagement.h"
#include "record.h"
#include <vector>
#include <chrono>
#include <winrt/Windows.Foundation.Collections.h>

struct RecordFilter {
private:
    static RecordFilter* instance;
    time_t dateFrom;
    time_t dateTo;
    int deviceIdx;
    int algorithmIdx;

    RecordFilter() : dateFrom(-1), dateTo(-1), deviceIdx(3), algorithmIdx(2) {}

public:
    static RecordFilter* getInstance() {
        if (instance == nullptr) {
            instance = new RecordFilter();
        }
        return instance;
    }

    void setDateFrom(const time_t date) { dateFrom = date; }
    void setDateTo(const time_t date) { dateTo = date; }
    void setDeviceIdx(int idx) { deviceIdx = idx; }
    void setAlgorithmIdx(int idx) { algorithmIdx = idx; }

    bool filter(Record& record) {
		time_t recordDate = record.getExecutionDate();

        if (dateFrom != -1 && dateFrom > recordDate) {
			return false;
        }

        if (dateTo != -1 && dateTo <= recordDate) {
            return false;
        }

        if (deviceIdx != 3 && deviceIdx != record.getDeviceIdx()) {
            return false;
        }

        if (algorithmIdx != 2 && algorithmIdx != record.getAlgorithmIdx()) {
            return false;
        }

        return true;
    }
};


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

            for (auto record = records.rbegin(); record != records.rend(); record++) {
                displayRecords.Append(RecordViewModel::Create(
                    record->getId(),
                    winrt::to_hstring(record->getExecutionDateStr()),
                    record->getDeviceIdx(),
                    winrt::to_hstring(record->getDeviceName()),
                    record->getAlgorithmIdx(),
                    winrt::to_hstring(record->getAlgorithmName()),
                    record->getTimeSpend(),
                    winrt::to_hstring(record->getDetails().dump())
                ));
            }
        }

        Windows::Foundation::Collections::IObservableVector<FYP_V2::RecordViewModel> Records() const;

    private:
        Windows::Foundation::Collections::IObservableVector<FYP_V2::RecordViewModel> displayRecords;
        int clickedItemId = -1;
    public:
        void ListView_ItemClick(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Controls::ItemClickEventArgs const& e);
        void RemoveBtn_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void SearchBtn_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void DateFrom_DateChanged(winrt::Microsoft::UI::Xaml::Controls::CalendarDatePicker const& sender, winrt::Microsoft::UI::Xaml::Controls::CalendarDatePickerDateChangedEventArgs const& args);
        void DateTo_DateChanged(winrt::Microsoft::UI::Xaml::Controls::CalendarDatePicker const& sender, winrt::Microsoft::UI::Xaml::Controls::CalendarDatePickerDateChangedEventArgs const& args);
        void DeviceFilter_SelectionChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs const& e);
        void AlgorithmFilter_SelectionChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs const& e);

        void ExportBtn_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
    };
}

namespace winrt::FYP_V2::factory_implementation
{
    struct RecordPage : RecordPageT<RecordPage, implementation::RecordPage>
    {
    };
}
