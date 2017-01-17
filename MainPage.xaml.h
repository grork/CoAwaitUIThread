#pragma once

#include "MainPage.g.h"
#include "AwaitHelperWinRT.h"

namespace CoAwaitUIThread
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	public ref class MainPage sealed
	{
	public:
		MainPage();

    private:
        void Start_Thread_End_UI(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void Start_Thread_Dont_End_UI(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void LogSomething(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

        void LogToWindow(Platform::String^ data);

        await_winrthelper::thread_context ui_thread;
        concurrency::task<void> Work(bool completedOnUI);
    };
}
