#include "pch.h"
#include "MainPage.xaml.h"
#include <experimental\resumable>
#include <pplawait.h>

using namespace CoAwaitUIThread;
using namespace concurrency;
using namespace Platform;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::Storage;

MainPage::MainPage()
{
    InitializeComponent();
}

void MainPage::Start_Thread_End_UI(Object^ sender, RoutedEventArgs^ e)
{
    // Bounce to some random thread (E.g. not UI)
    // This helps to ensure that operations won't complete on
    // the UI thread naturally
    create_task([this] {
        this->Work(true);
    });
}

void MainPage::Start_Thread_Dont_End_UI(Object^ sender, RoutedEventArgs^ e)
{
    create_task([this] {
        this->Work(false);
    });
}

concurrency::task<void> MainPage::Work(bool completeOnUI)
{
    // This intended to demonstrate using co_await on some async
    // operations that will do "work" somewhere in the system on other threads.
    auto file = co_await ApplicationData::Current->LocalFolder->CreateFileAsync("testFile.txt", CreationCollisionOption::ReplaceExisting);
    co_await FileIO::WriteTextAsync(file, "TestData");

    if (completeOnUI)
    {
        // Here, if we're attempting to complete on the UI
        // we should wait on the magic cookie we captured
        // as memember
        co_await this->ui_thread;
    }

    // Capture if we have access, and log
    // the state that we have found we're on
    bool hasThreadAccess = this->Dispatcher->HasThreadAccess;
    if (!hasThreadAccess)
    {
        // Since we're not on the thread, we have to just queue some
        // work for the UI thread to actually log to the dispatcher
        this->Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, ref new Windows::UI::Core::DispatchedHandler([this]()
        {
            this->LogToWindow(L"Did not complete on the UI thread");
        }));
    }
    else
    {
        // We're on the UI thread, so we can just access it.
        this->LogToWindow(L"Completed on the UI thread");
    }
}

void MainPage::LogSomething(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    LogToWindow(L"Logging works");
}

void MainPage::LogToWindow(String^ data)
{
    TextBlock^ logLine = ref new TextBlock();
    logLine->Text = data;

    this->LogOutput->Children->InsertAt(0, logLine);
}
