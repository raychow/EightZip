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
            auto isFinished = bool { false };
            std::condition_variable cv {};
            std::mutex mtx {};
            std::unique_lock < std::mutex > ul { mtx };
            wxTheApp->CallAfter([&]() {
                fun();
                isFinished = true;
                cv.notify_one();
            });
            cv.wait(ul, [&] { return isFinished; });
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
