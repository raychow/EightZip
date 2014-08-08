#include "FileStream.h"

#include "Exception.h"
#include "IArchiveAdapter.h"

using namespace std;

namespace SevenZipCore
{
    void InFileStream::Open(const TString &tstrPath, bool isShareWrite)
{
        m_file.Open(tstrPath, isShareWrite);
    }

    STDMETHODIMP InFileStream::Read(
        void *pData, UINT32 unSize, UINT32 *punProcessedSize)
    {
        try
        {
            if (punProcessedSize)
            {
                *punProcessedSize = 0;
            }
            if (0 == unSize)
            {
                return S_OK;
            }

            auto unBytesRead = m_file.ReadPart(
                reinterpret_cast<BYTE *>(pData), unSize);

            if (punProcessedSize)
            {
                *punProcessedSize = unBytesRead;
            }
            return S_OK;
        }
        catch (...)
        {
            return E_FAIL;
        }
    }

    STDMETHODIMP InFileStream::Seek(
        INT64 n64Offset,
        UINT32 unSeekOrigin,
        UINT64 *pun64NewPosition)
    {
        IOFile::SeekOrigin seekOrigin;
        switch (unSeekOrigin)
        {
        case STREAM_SEEK_SET:
            seekOrigin = IOFile::SeekOrigin::Begin;
            break;
        case STREAM_SEEK_CUR:
            seekOrigin = IOFile::SeekOrigin::Current;
            break;
        case STREAM_SEEK_END:
            seekOrigin = IOFile::SeekOrigin::End;
            break;
        default:
            return E_INVALIDARG;
        }
        try
        {
            auto n64NewPosition = m_file.Seek(n64Offset, seekOrigin);

            if (pun64NewPosition)
            {
                *pun64NewPosition = n64NewPosition;
            }
            return S_OK;
        }
        catch (...)
        {
            return E_FAIL;
        }
    }

    STDMETHODIMP InFileStream::GetSize(UINT64 *pun64Size)
    {
        try
        {
            if (pun64Size)
            {
                *pun64Size = m_file.GetSize();
            }
            return S_OK;
        }
        catch (...)
        {
            return E_INVALIDARG;
        }
    }

    IMPLEMENT_ADAPTER_CONSTRUCTOR(InFileStream)
    IMPLEMENT_IINSTREAM_ADAPTER(InFileStream)
    IMPLEMENT_ISTREAMGETSIZE_ADAPTER(InFileStream)

    void OutFileStream::Open(const TString &tstrPath, bool isTruncate)
    {
         m_file.Open(tstrPath, isTruncate);
         m_un64ProcessedSize = 0;
    }

    void OutFileStream::Close()
    {
        m_file.Close();
    }

    STDMETHODIMP OutFileStream::Write(
        const void *pData, UINT32 unSize, UINT32 *punProcessedSize)
    {
        try
        {
            if (punProcessedSize)
            {
                *punProcessedSize = 0;
            }
            if (0 == unSize)
            {
                return S_OK;
            }

            auto unBytesRead = m_file.WritePart(
                reinterpret_cast<const BYTE *>(pData), unSize);

            m_un64ProcessedSize += unBytesRead;
            if (punProcessedSize)
            {
                *punProcessedSize = unBytesRead;
            }
            return S_OK;
        }
        catch (...)
        {
            return E_FAIL;
        }
    }

    STDMETHODIMP OutFileStream::Seek(
        INT64 n64Offset, UINT32 unSeekOrigin, UINT64 *pun64NewPosition)
    {
        IOFile::SeekOrigin seekOrigin;
        switch (unSeekOrigin)
        {
        case STREAM_SEEK_SET:
            seekOrigin = IOFile::SeekOrigin::Begin;
            break;
        case STREAM_SEEK_CUR:
            seekOrigin = IOFile::SeekOrigin::Current;
            break;
        case STREAM_SEEK_END:
            seekOrigin = IOFile::SeekOrigin::End;
            break;
        default:
            return E_INVALIDARG;
        }
        try
        {
            auto n64NewPosition = m_file.Seek(n64Offset, seekOrigin);

            if (pun64NewPosition)
            {
                *pun64NewPosition = n64NewPosition;
            }
            return S_OK;
        }
        catch (...)
        {
            return E_FAIL;
        }
    }

    STDMETHODIMP OutFileStream::SetSize(UINT64 un64Size)
    {
        try
        {
            m_file.SetSize(un64Size);
            return S_OK;
        }
        catch (...)
        {
            return E_INVALIDARG;
        }
    }

    void OutFileStream::SetTime(
        const FILETIME *pftCreated,
        const FILETIME *pftAccessed,
        const FILETIME *pftModified)
    {
        m_file.SetTime(pftCreated, pftAccessed, pftModified);
    }

    IMPLEMENT_ADAPTER_CONSTRUCTOR(OutFileStream)
    IMPLEMENT_OUTFILESTREAM_ADAPTER(OutFileStream)

}
