﻿#include "pch.h"
#include "HomePage.xaml.h"
#include "../fileManagement.h"
#include <fstream>
#if __has_include("HomePage.g.cpp")
#include "HomePage.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::FYP_V2::implementation
{
    int32_t HomePage::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void HomePage::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }
}

void winrt::FYP_V2::implementation::HomePage::ImportVertices(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
{
    std::string filename = fileFunctions::OpenFileDialog();
    if (fileFunctions::isCSVFile) {
        std::ifstream nodeFile(filename);

        currentNodes = fileFunctions::readNodeFile(nodeFile);
    }
}

void winrt::FYP_V2::implementation::HomePage::ImportEdges(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
{
    std::string filename = fileFunctions::OpenFileDialog();
    if (fileFunctions::isCSVFile) {
        std::ifstream edgeFile(filename);

        currentEdges = fileFunctions::readEdgeFile(edgeFile, currentNodes);
    }
}
