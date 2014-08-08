#include "IOFile.h"

#include <cassert>

#ifndef __WINDOWS__
// Use 64bit data type for file offset.
#define _FILE_OFFSET_BITS 64
#define _LARGEFILE64_SOURCE

#include <stdio.h>
#include <sys/types.h>
#include <utime.h>
#endif
#include "Exception.h"

namespace SevenZipCore
{
    void IOFile::OpenForInput(TString tstrPath, bool isShareWrite)
    {
#ifdef __WINDOWS__
        m_upFile.reset(::CreateFile(
            tstrPath.c_str(),
            GENERIC_READ,
            FILE_SHARE_READ | (isShareWrite ? FILE_SHARE_WRITE : 0),
            nullptr,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            nullptr));
        if (INVALID_HANDLE_VALUE == m_upFile.get())
        {
            throw FileException("Cannot open file.");
        }
#else
        // TODO: fcntl() or flock()?
        m_upFile.reset(fopen(
            ConvertTStringToString(tstrPath).c_str(), "rb"));
        if (!m_upFile)
        {
            throw FileException("Cannot open file.");
        }
#endif
        m_tstrPath = tstrPath;
    }

    void IOFile::OpenForOutput(TString tstrPath, bool isTruncate)
    {
#ifdef __WINDOWS__
        m_upFile.reset(::CreateFile(
            tstrPath.c_str(),
            GENERIC_WRITE,
            FILE_SHARE_READ,
            nullptr,
            isTruncate ? CREATE_ALWAYS : OPEN_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            nullptr));
        if (INVALID_HANDLE_VALUE == m_upFile.get())
        {
            throw FileException("Cannot open file.");
        }
#else
        // TODO: fcntl() or flock()?
        m_upFile.reset(fopen(ConvertTStringToString(
            tstrPath).c_str(), isTruncate ? "wb+" : "wb"));
        if (!m_upFile)
        {
            throw FileException("Cannot open file.");
        }
#endif
        m_tstrPath = tstrPath;
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

    UINT64 IOFile::GetSize() const
    {
#ifdef __WINDOWS__
        LARGE_INTEGER result;
        if (0 == GetFileSizeEx(m_upFile.get(), &result))
        {
            throw FileException("Cannot get the size of the file.");
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

    void InFile::Read(BYTE *pbyBuffer, UINT32 nBytesToRead) const
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

    UINT32 InFile::ReadPart(BYTE *pbyBuffer, UINT32 nBytesToRead) const
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

    void OutFile::Write(const BYTE *pbyBuffer, UINT32 nBytesToWrite) const
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

    UINT32 OutFile::WritePart(const BYTE *pbyBuffer, UINT32 nBytesToWrite) const
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

    void OutFile::SetTime(
        const FILETIME *pftCreated,
        const FILETIME *pftAccessed,
        const FILETIME *pftModified)
    {
#ifdef __WINDOWS__
        if (0 == ::SetFileTime(
            m_upFile.get(), pftCreated, pftAccessed, pftModified))
        {
            throw FileException("Cannot set file time.");
        }
#else
        struct utimbuf timeBuffer = {};
        if (!pftAccessed || !pftModified)
        {
            struct stat statBuffer = {};
            if (0 != fstat(fileno(m_upFile.get()), &statBuffer)))
            {
                throw FileException("Cannot set file time.");
            }
            timeBuffer.actime = statBuffer.st_atime;
            timeBuffer.modtime = statBuffer.st_mtime;
        }
        if (pftAccessed)
        {
            timeBuffer.actime = Helper::GetUnixTimeFromFileTime(*pftAccessed);
        }
        if (pftModified)
        {
            timeBuffer.modtime = Helper::GetUnixTimeFromFileTime(*pftModified);
        }
        if (0 != utime(ConvertTStringToString(m_tstrPath).c_str(), &timeBuffer))
        {
            throw FileException("Cannot set file time.");
        }
#endif
    }

    void OutFile::SetSize(UINT64 un64Size) const
    {
#ifdef __WINDOWS__
        if (!(un64Size == Seek(un64Size, IOFile::SeekOrigin::Begin)
            && 0 != ::SetEndOfFile(m_upFile.get())))
#else
        if (0 != ftruncate(fileno(m_upFile.get()), un64Size))
#endif
        {
            throw FileException("Cannot set file size.");
        }
    }

}