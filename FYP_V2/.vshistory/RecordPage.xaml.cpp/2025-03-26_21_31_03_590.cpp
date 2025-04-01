#include "pch.h"
#include "RecordPage.xaml.h"
#include "fileManagement.h"
#include "SharedData.h"
#include "ExcelResult.h"
#include <sstream>
#if __has_include("RecordPage.g.cpp")
#include "RecordPage.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

RecordFilter* RecordFilter::instance = nullptr;
constexpr int64_t epochDifference = 116444736000000000LL;

enum algorithmType {
    ParallelDijkstra, _2DPartitioningBFS
};

namespace winrt::FYP_V2::implementation
{
    Windows::Foundation::Collections::IObservableVector<FYP_V2::RecordViewModel> RecordPage::Records() const
    {
        return displayRecords;
    }
}

void winrt::FYP_V2::implementation::RecordPage::ListView_ItemClick(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Controls::ItemClickEventArgs const& e)
{
    auto clickedItem = e.ClickedItem().try_as<FYP_V2::RecordViewModel>();

    if (clickedItem)
    {
        clickedItemId = clickedItem.Id();
    }
}


void winrt::FYP_V2::implementation::RecordPage::RemoveBtn_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
{
    if (clickedItemId == -1) {
        Controls::ContentDialog warningDialog;
        warningDialog.XamlRoot(this->XamlRoot());
        warningDialog.Title(winrt::box_value(hstring(L"Warning")));
        warningDialog.Content(winrt::box_value(hstring(L"Please select a record!")));
        warningDialog.PrimaryButtonText(hstring(L"Yes"));

        warningDialog.ShowAsync();
        return;
    }

    Controls::ContentDialog confirmDialog;
    confirmDialog.XamlRoot(this->XamlRoot());
    confirmDialog.Title(winrt::box_value(hstring(L"Confirmation")));
    confirmDialog.Content(winrt::box_value(hstring(L"Are you sure?")));
    confirmDialog.PrimaryButtonText(hstring(L"Yes"));
    confirmDialog.CloseButtonText(hstring(L"Cancel"));

    confirmDialog.PrimaryButtonClick([this](winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::Controls::ContentDialogButtonClickEventArgs const&) {
        for (uint32_t i = 0; i < displayRecords.Size();i++) {
			auto record = displayRecords.GetAt(i);
			if (record.Id() == clickedItemId) {
                fileFunctions::removeRecordFile(clickedItemId);
				displayRecords.RemoveAt(i);
                return;
			}
        }
    });

	confirmDialog.ShowAsync();
}

void winrt::FYP_V2::implementation::RecordPage::SearchBtn_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
{
	displayRecords.Clear();
	auto filter = RecordFilter::getInstance();
	std::vector<Record> records = fileFunctions::getAllRecords();
    for (auto record = records.rbegin(); record != records.rend();record++) {
		if (filter->filter(*record)) {
			displayRecords.Append(RecordViewModel::Create(
				record->getId(),
				winrt::to_hstring(record->getExecutionDateStr()),
                record->getDeviceIdx(),
				winrt::to_hstring(record->getDeviceName()),
				record->getAlgorithmIdx(),
				winrt::to_hstring(record->getAlgorithmName()),
				record->getTimeSpend(),
                record->getNodeList(),
				winrt::to_hstring(record->getDetails().dump())
			));
		}
	}
}

void winrt::FYP_V2::implementation::RecordPage::DateFrom_DateChanged(winrt::Microsoft::UI::Xaml::Controls::CalendarDatePicker const& sender, winrt::Microsoft::UI::Xaml::Controls::CalendarDatePickerDateChangedEventArgs const& args)
{
    auto filter = RecordFilter::getInstance();
    if (args.NewDate()) {
        auto date = args.NewDate().Value().time_since_epoch().count();

        char dateStr[20];

        time_t dateT = (date - epochDifference) / 10'000'000;

        filter->setDateFrom(dateT);
    } else {
        filter->setDateFrom(-1);
    }
}

void winrt::FYP_V2::implementation::RecordPage::DateTo_DateChanged(winrt::Microsoft::UI::Xaml::Controls::CalendarDatePicker const& sender, winrt::Microsoft::UI::Xaml::Controls::CalendarDatePickerDateChangedEventArgs const& args)
{
    auto filter = RecordFilter::getInstance();
    if (args.NewDate()) {
        auto date = args.NewDate().Value().time_since_epoch().count();

        char dateStr[20];

        time_t dateT = (date - epochDifference) / 10'000'000;

        filter->setDateTo(dateT);
    } else {
		filter->setDateTo(-1);
    }
}

void winrt::FYP_V2::implementation::RecordPage::DeviceFilter_SelectionChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs const& e)
{
	auto filter = RecordFilter::getInstance();
	
	winrt::Microsoft::UI::Xaml::Controls::ComboBox combox = sender.as<winrt::Microsoft::UI::Xaml::Controls::ComboBox>();
	int idx = combox.SelectedIndex();

    filter->setDeviceIdx(idx);
}

void winrt::FYP_V2::implementation::RecordPage::AlgorithmFilter_SelectionChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs const& e)
{
    auto filter = RecordFilter::getInstance();

    winrt::Microsoft::UI::Xaml::Controls::ComboBox combox = sender.as<winrt::Microsoft::UI::Xaml::Controls::ComboBox>();
    int idx = combox.SelectedIndex();

    filter->setAlgorithmIdx(idx);
}
void winrt::FYP_V2::implementation::RecordPage::ExportBtn_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
{
    if (clickedItemId == -1) {
        Controls::ContentDialog warningDialog;
        warningDialog.XamlRoot(this->XamlRoot());
        warningDialog.Title(winrt::box_value(hstring(L"Warning")));
        warningDialog.Content(winrt::box_value(hstring(L"Please select a record!")));
        warningDialog.PrimaryButtonText(hstring(L"Yes"));

        warningDialog.ShowAsync();
        return;
    }

    FYP_V2::RecordViewModel record{ nullptr };

    for (uint32_t i = 0; i < displayRecords.Size(); i++) {
        FYP_V2::RecordViewModel tempRecord = displayRecords.GetAt(i);
        if (tempRecord.Id() == clickedItemId) {
            record = tempRecord;
        }
    }

    if (clickedItemId == algorithmType::ParallelDijkstra) {
        auto er = DijkstraExcel();
        er.setRecord(record);
        er.setNodeList(SharedData::getInstance().currentNodes);
        er.exportResult();
    }
}
