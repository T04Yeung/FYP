#include "pch.h"
#include "MainWindow.xaml.h"
#include <winrt/Windows.UI.Xaml.Interop.h>
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::FYP_V2::implementation
{
	MainWindow::MainWindow()
	{
		InitializeComponent();
		MainWindowFrame().Navigate(winrt::xaml_typename<FYP_V2::HomePage>());
        Nav().Translation({0, 0, 64});
	}
    int32_t MainWindow::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void MainWindow::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }
}
