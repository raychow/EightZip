#include "FileStream.h"

#include "Exception.h"
#include "IArchiveAdapter.h"

using namespace std;

namespace SevenZipCore
{
    InFileStream::InFileStream(const TString &tstrPath)
        : InFileStream()
    {
        Open(tstrPath);
    }

    void InFileStream::Open(const TString &tstrPath)
    {
        m_ifFile.open(
            ConvertWStringToString(tstrPath),
            ios_base::in | ios_base::binary);
        if (!m_ifFile.good())
        {
            throw StreamException("Cannot open specified file.");
        }
    }

    STDMETHODIMP InFileStream::Read(
        void *data, UINT32 size, UINT32 *processedSize)
    {
        if (processedSize)
        {
            *processedSize = 0;
        }
        if (0 == size)
        {
            return S_OK;
        }
        // TODO: if isDeviceFile
        // ********** CHECK THIS **********
        // 7zFM, FileIO.cpp, line 315:
        // ReadFile and WriteFile functions in Windows have BUG:
        // If you Read or Write 64MB or more (probably min_failure_size = 64MB - 32KB + 1)
        // from/to Network file, it returns ERROR_NO_SYSTEM_RESOURCES
        // (Insufficient system resources exist to complete the requested service).
        //
        // Probably in some version of Windows there are problems with other sizes:
        // for 32 MB (maybe also for 16 MB).
        // And message can be "Network connection was lost"
        // else
        m_ifFile.read(reinterpret_cast<char *>(data), size);
        // When read beyond the end, the fail bit is set,
        // but we can ignore it because data is set.
        if (m_ifFile.bad())
        {
            return S_FALSE;
        }
        if (processedSize)
        {
            *processedSize = static_cast<UINT32>(m_ifFile.gcount());
        }
        return S_OK;
    }

    STDMETHODIMP InFileStream::Seek(
        INT64 offset,
        UINT32 seekOrigin,
        UINT64 *newPosition)
    {
        ios_base::seekdir seekDirection;
        switch (seekOrigin)
        {
        case STREAM_SEEK_SET:
            seekDirection = ios_base::beg;
            break;
        case STREAM_SEEK_CUR:
            seekDirection = ios_base::cur;
            break;
        case STREAM_SEEK_END:
            seekDirection = ios_base::end;
            break;
        default:
            return STG_E_INVALIDFUNCTION;
        }
        m_ifFile.seekg(offset, seekDirection);
        // fail() checks both fail and bad bits.
        if (m_ifFile.fail())
        {
            return S_FALSE;
        }
        if (newPosition)
        {
            *newPosition = m_ifFile.tellg();
        }
        return S_OK;
    }

    STDMETHODIMP InFileStream::GetSize(UINT64 *size)
    {
        auto origPosition = m_ifFile.tellg();
        m_ifFile.seekg(0, ios_base::end);
        *size = m_ifFile.tellg();
        m_ifFile.seekg(origPosition, ios_base::beg);
        if (m_ifFile.fail())
        {
            return S_FALSE;
        }
        return S_OK;
    }

    IMPLEMENT_ADAPTER_CONSTRUCTOR(InFileStream)
    IMPLEMENT_IINSTREAM_ADAPTER(InFileStream)
    IMPLEMENT_ISTREAMGETSIZE_ADAPTER(InFileStream)
}
