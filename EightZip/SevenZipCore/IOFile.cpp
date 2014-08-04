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
            throw FileException("Cannot open file.");
        }
#else
        // TODO: fcntl() or flock()?
        m_upFile.reset(fopen(
            ConvertTStringToString(tstrPath).c_str(), isInput ? "r" : "w"));
        if (!m_upFile)
        {
            throw FileException("Cannot open file.");
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
            throw FileException("Cannot get position of the file.");
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
            throw FileException("Cannot get length of the file.");
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
            throw FileException("Cannot seek in the file.");
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
        throw FileException("Cannot seek in the file.");
#endif
    }

    void InFile::Read(BYTE *pbyBuffer, UINT32 nBytesToRead)
    {
        UINT32 unBytesProcessed = 0;
        while (unBytesProcessed < nBytesToRead)
        {
            auto unBytesRead = ReadPart(pbyBuffer, nBytesToRead);
            if (!unBytesRead)
            {
                return;
            }
            pbyBuffer += unBytesRead;
            unBytesProcessed += unBytesRead;
        }
    }

    UINT32 InFile::ReadPart(BYTE *pbyBuffer, UINT32 nBytesToRead)
    {
        if (nBytesToRead > MAX_CHUNK_SIZE)
        {
            nBytesToRead = MAX_CHUNK_SIZE;
        }
        DWORD result = 0;
#ifdef __WINDOWS__
        if (0 == ::ReadFile(
            m_upFile.get(), pbyBuffer, nBytesToRead, &result, nullptr))
#else
        result = fread(pbyBuffer, 1, nBytesToRead, m_upFile.get());
        if (ferror(m_upFile.get()))
#endif
        {
            throw FileException("Cannot read from the file.");
        }
        return result;
    }

    void OutFile::Write(BYTE *pbyBuffer, UINT32 nBytesToWrite)
    {
        UINT32 unBytesProcessed = 0;
        while (unBytesProcessed < nBytesToWrite)
        {
            auto unBytesWrite = WritePart(pbyBuffer, nBytesToWrite);
            if (!unBytesWrite)
            {
                return;
            }
            pbyBuffer += unBytesWrite;
            unBytesProcessed += unBytesWrite;
        }
    }

    UINT32 OutFile::WritePart(BYTE *pbyBuffer, UINT32 nBytesToWrite)
    {
        if (nBytesToWrite > MAX_CHUNK_SIZE)
        {
            nBytesToWrite = MAX_CHUNK_SIZE;
        }
        DWORD result = 0;
#ifdef __WINDOWS__
        if (0 == ::WriteFile(
            m_upFile.get(), pbyBuffer, nBytesToWrite, &result, nullptr))
#else
        result = fwrite(pbyBuffer, 1, nBytesToWrite, m_upFile.get());
        if (ferror(m_upFile.get()))
#endif
        {
            throw FileException("Cannot write to the file.");
        }
        return result;
    }

    void OutFile::SetLength(UINT64 un64Length)
    {
#ifdef __WINDOWS__
        if (!(un64Length == Seek(un64Length, IOFile::SeekOrigin::Begin)
            && 0 != ::SetEndOfFile(m_upFile.get())))
#else
        if (0 != ftruncate(fileno(m_upFile.get()), un64Length))
#endif
        {
            throw FileException("Cannot set file length.");
        }
    }

}