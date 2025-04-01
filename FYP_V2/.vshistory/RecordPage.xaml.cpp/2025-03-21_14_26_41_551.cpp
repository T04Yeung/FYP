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
