#include "pch.h"
#include "RecordPage.xaml.h"
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
    Controls::ContentDialog dialog;
    dialog.XamlRoot(this->XamlRoot());
    dialog.Title(winrt::box_value(hstring(L"Confirmation")));
    dialog.Content(winrt::box_value(hstring(L"Are you sure?")));
    dialog.PrimaryButtonText(hstring(L"Yes"));
	dialog.SecondaryButtonText(hstring(L"Cancel"));

    dialog.ShowAsync();
}
