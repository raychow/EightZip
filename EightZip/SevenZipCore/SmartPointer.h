// SmartPointer.h

#ifndef SEVENZIPCORE_SMARTPOINTER_H
#define SEVENZIPCORE_SMARTPOINTER_H

#include <memory>

#include "Platform.h"

#ifdef __WINDOWS__

class HandleCloser
{
public:
    void operator()(HANDLE handle)
    {
        if (INVALID_HANDLE_VALUE != handle)
        {
            CloseHandle(handle);
        }
    }
};

#endif

class FileCloser
{
public:
    void operator()(FILE *pFile)
    {
        if (pFile)
        {
            fclose(pFile);
        }
    }
};

#ifdef __WINDOWS__
typedef std::unique_ptr<void, HandleCloser> HandleUniquePtr;
#endif

typedef std::unique_ptr<void, FileCloser> FileUniquePtr;

#endif // SEVENZIPCORE_SMARTPOINTER_H
