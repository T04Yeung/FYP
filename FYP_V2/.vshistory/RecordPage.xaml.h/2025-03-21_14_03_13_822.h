#pragma once

#include "RecordPage.g.h"
#include "fileManagement.h"
#include "record.h"
#include <vector>
#include <winrt/Windows.Foundation.Collections.h>

namespace winrt::FYP_V2::implementation
{
    struct RecordPage : RecordPageT<RecordPage>
    {
        RecordPage()
        {
            InitializeComponent();
            SearchBarContainer().Translation({0, 0, 32});

            std::vector<Record> records = fileFunctions::getAllRecords();
        }

        std::vector<Record> Records() const;

    private:
        std::vector<Record> records;
    };
}

namespace winrt::FYP_V2::factory_implementation
{
    struct RecordPage : RecordPageT<RecordPage, implementation::RecordPage>
    {
    };
}
