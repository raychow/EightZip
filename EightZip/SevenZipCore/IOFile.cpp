#include "IOFile.h"

#include <cassert>

#ifdef __WINDOWS__
#include <Windows.h>
#else
// Use 64bit data type for file offset.
#define _FILE_OFFSET_BITS 64
#define _LARGEFILE64_SOURCE

#include <stdio.h>
#endif

#include "Exception.h"

namespace SevenZipCore
{
    void IOFile::Open(TString tstrPath, bool isInput)
    {
#ifdef __WINDOWS__
        m_upFile.reset(::CreateFile(
            tstrPath.c_str(),
            isInput ? GENERIC_READ : GENERIC_WRITE,
            FILE_SHARE_READ,
            nullptr,
            isInput ? OPEN_EXISTING : CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            nullptr));
        if (INVALID_HANDLE_VALUE == m_upFile.get())
        {
            throw SystemException("Cannot open file.");
        }
#else
        m_upFile.reset(fopen(ConvertTStringToString(tstrPath).c_str(), isInput ? "r" : "w"));
        if (!m_upFile)
        {
            throw SystemException("Cannot open file.");
        }
#endif
    }

    UINT64 IOFile::GetPosition() const
    {
#ifdef __WINDOWS__
        return Seek(0, SeekOrigin::Current);
#else
        auto result = ftell(m_upFile.get());
        if (-1 == result)
        {
            throw SystemException("Can not get position.");
        }
        return result;
#endif
    }

    UINT64 IOFile::GetLength() const
    {
#ifdef __WINDOWS__
        LARGE_INTEGER result;
        if (0 == GetFileSizeEx(m_upFile.get(), &result))
        {
            throw SystemException("Can not get length.");
        }
        return result.QuadPart;
#else
        auto oldPosition = GetPosition();
        auto result = Seek(0, SeekOrigin::End);
        Seek(oldPosition, SeekOrigin::Begin);
        return result;
#endif
    }

    INT64 IOFile::Seek(INT64 n64Offset, SeekOrigin seekOrigin) const
    {
#ifdef __WINDOWS__
        DWORD dwMoveMethod = 0;
        switch (seekOrigin)
        {
        case SeekOrigin::Begin:
            dwMoveMethod = FILE_BEGIN;
            break;
        case SeekOrigin::Current:
            dwMoveMethod = FILE_CURRENT;
            break;
        case SeekOrigin::End:
            dwMoveMethod = FILE_END;
            break;
        default:
            assert(false);
            return -1;
        }
        LARGE_INTEGER result;
        result.QuadPart = n64Offset;
        if (0 == ::SetFilePointerEx(
            m_upFile.get(),
            result,
            &result,
            dwMoveMethod))
        {
            throw SystemException("Can not seek.");
        }
        return result.QuadPart;
#else
        int nOrigin = 0;
        switch (seekOrigin)
        {
        case SeekOrigin::Begin:
            nOrigin = SEEK_SET;
            break;
        case SeekOrigin::Current:
            nOrigin = SEEK_CUR;
            break;
        case SeekOrigin::End:
            nOrigin = SEEK_END;
            break;
        default:
            assert(false);
            return -1;
        }
        if (0 == fseek(m_upFile.get(), n64Offset, nOrigin))
        {
            auto result = ftell(m_upFile.get());
            if (-1 != result)
            {
                return result;
            }
        }
        throw SystemException("Can not seek.");
#endif
    }

}