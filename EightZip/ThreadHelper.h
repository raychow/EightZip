// ThreadHelper.h

#ifndef THREADHELPER_H
#define THREADHELPER_H

#include <condition_variable>
#include <future>

namespace Helper
{
    template<typename T>
    void InvokeOnMainThread(T fun)
    {
        if (wxThread::IsMain())
        {
            fun();
        }
        else
        {
            std::condition_variable cv {};
            wxTheApp->CallAfter([&]() {
                fun();
                cv.notify_one();
            });
            std::mutex mtx {};
            std::unique_lock < std::mutex > ul { mtx };
            cv.wait(ul);
        }
    }

    template<typename T>
    auto CallOnMainThread(T fun) -> decltype(fun())
    {
        if (wxThread::IsMain())
        {
            return fun();
        }
        else
        {
            auto result = std::promise < decltype(fun()) > {};
            wxTheApp->CallAfter([&]() {
                result.set_value(fun());
            });
            return result.get_future().get();
        }
    }

}

#endif // THREADHELPER_H
