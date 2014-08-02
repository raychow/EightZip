// HandleUniquePtr.h

#ifndef HANDLEUNIQUEPTR_H
#define HANDLEUNIQUEPTR_H

#ifdef __WXMSW__

#include <memory>

class HandleCloser
{
public:
    void operator()(HANDLE handle)
    {
        if (handle != INVALID_HANDLE_VALUE)
        {
            CloseHandle(handle);
        }
    }
};
#endif

typedef std::unique_ptr<void, HandleCloser> HandleUniquePtr;

#endif // HANDLEUNIQUEPTR_H