#include "pch.h"
#include "RecordViewModel.h"
#if __has_include("RecordViewModel.g.cpp")
#include "RecordViewModel.g.cpp"
#endif

namespace winrt::FYP_V2::implementation
{
    int32_t RecordViewModel::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void RecordViewModel::MyProperty(int32_t /*value*/)
    {
        throw hresult_not_implemented();
    }
}
