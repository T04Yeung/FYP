#include "pch.h"
#include "RecordPage.xaml.h"
#include "fileManagement.h"
#if __has_include("RecordPage.g.cpp")
#include "RecordPage.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

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
