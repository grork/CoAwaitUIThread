CoAwaitUIThread
===============

This project is an example of how to wrangle `co_await` with C++/CX into a situation where it can be used to enforce contuation of the work on the UI Thread.

#### Usage
You need to `co_await` onto the UI thread? (Or, any specific thread). To do that, you need to allocate the `thread_context`, and then await on that when you need to continue on the UI thread.

    #include "AwaitHelperWinRT.h"

    ref class MyControl sealed // Allocate me on the UI thread
    {
        await_asynchelper::thread_context ui_thread;

        event Windows::UI::Xaml::RoutedEventHandler^ MyEvent;
        concurrency::task<void> DoWork() // Call me somewhere else thats not the UI thread
        {
            co_await ExpensiveWork();
            co_await this->ui_thread;
            this->MyEvent(this, null); // Completes on UI thread
        }
    }

You can see simple usage of this in the app that is included here. Take a look, click both buttons, and hopefully that will help clearify whats going on.

#### Background
Normally, when working with the PPL, one would provide a [continuation context](https://msdn.microsoft.com/en-us/library/hh749968.aspx) to the continuation specifying where that block would complete.

When working with `co_routines` & `co_await`, the implementation of the PPL to `co_await` adapter defined in `pplawait.h`, will provide you behaviour similar to `use_default`. e.g. if you start on the UI thread, it'll end on the UI thread. Great! But what if you need to ensure tha while your thread is initiated from any thread, you need it to get back to the UI thread? Turns out that is a pain in the rear.

Example 1:

    task<bool> GetAnAnswer();

    void MyWork()
    {
        GetAnswer().then([] {
			// Will execute in the apartment where .then was invoked
        )); // Default it tast_continuation_context::use_default
    }

Example 2:

    task<bool> GetAnAnswer();

    void MyWork()
    {
        GetAnswer().then([] {
			// Will execute wherever the PPL decides
        ), task_continuation_context::use_arbitary);
    }

Example 3:

    task<bool> GetAnAnswer();

    void MyWork()
    {
        GetAnswer().then([] {
			// Will execute on the WinRT context, irrespective of where the prior task came from
        ), task_continuation_context::use_current);
    }

##### co_await, co_routines
At the time of writing, I don't think I could claim to be an expert in the wonders of `co_await` & friends. What I *do* know is that writing code using `co_await` is significantly cleaner & easier to read than the PPL & it's promise-like syntax.

Because of that I was writing some code, and I was trying to understand where & how the "continuation" of the co_routines were going to execute. After some diving, and consultation on [twitter](https://twitter.com/kennykerr/status/821086254443872256), I think I understand enough to leverage some code from [C++/WinRT](https://github.com/microsoft/cppwinrt), to allow me to creat the helper here.

#### Resources

I recommend watching these videos:

[Putting Coroutines to work with the windows runtime](https://www.youtube.com/watch?v=v0SjumbIips)

[C++ Coroutines: Under the covers](https://www.youtube.com/watch?v=8C8NnE1Dg4A)

Reading these blog posts:
[C++/WinRT: Fun With Agility](https://kennykerr.ca/2016/11/11/cppwinrt-fun-with-agility/)
[coroutines in Visual Studio 2015 Update 1](https://blogs.msdn.microsoft.com/vcblog/2015/11/30/coroutines-in-visual-studio-2015-update-1/)

If you're feeling particularily inclined to go _deep_:

[Wording for Coroutines (Or, the actual spec)](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0057r7.pdf)

[Coroutines: Reports from the field](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/p0054r0.html)