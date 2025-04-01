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

winrt::fire_and_forget winrt::FYP_V2::implementation::RecordPage::ShowMessage(hstring message) {
    Controls::ContentDialog dialog;
    dialog.XamlRoot(this->XamlRoot());
    dialog.Title(winrt::box_value(hstring(L"Message")));
    dialog.Content(winrt::box_value(message));
    dialog.PrimaryButtonText(hstring(L"OK"));

    co_await dialog.ShowAsync();
}
