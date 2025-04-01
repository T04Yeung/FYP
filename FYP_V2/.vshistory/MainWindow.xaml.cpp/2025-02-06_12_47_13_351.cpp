#include "pch.h"
#include "MainWindow.xaml.h"
#include <winrt/Windows.UI.Xaml.Interop.h>
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif
using namespace winrt::Microsoft::UI::Xaml::Controls;

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
		NavHomeItem().IsSelected(true);
		Nav().ItemInvoked({ this, &MainWindow::OnItemInvoked });
	}

    void MainWindow::OnItemInvoked(const NavigationView&, const NavigationViewItemInvokedEventArgs& args)
    {
        auto tag = args.InvokedItemContainer().Tag().as<hstring>();
        if (tag == L"HomePage")
        {
            MainWindowFrame().Navigate(winrt::xaml_typename<FYP_V2::HomePage>());
        }
        else if (tag == L"RecordPage")
        {

        }
    }
}
