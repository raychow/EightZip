#include "OutFileStream.h"

using namespace std;

namespace SevenZipCore
{

    STDMETHODIMP OutFileStream::Write(
        const void *data, UINT32 size, UINT32 *processedSize)
    {
        m_ofFile.write(reinterpret_cast<const char *>(data), size);
        if (!m_ofFile.good())
        {
            return E_FAIL;
        }
        return S_OK;
    }

    STDMETHODIMP OutFileStream::Seek(
        INT64 offset, UINT32 seekOrigin, UINT64 *newPosition)
    {
        ios_base::seekdir seekWay;
        switch (seekOrigin)
        {
        case FILE_BEGIN:
            seekWay = ios_base::beg;
            break;
        case FILE_CURRENT:
            seekWay = ios_base::cur;
            break;
        case FILE_END:
            seekWay = ios_base::end;
            break;
        default:
            return STG_E_INVALIDFUNCTION;
        }
        m_ofFile.seekp(offset, seekWay);
        if (newPosition)
        {
            *newPosition = m_ofFile.tellp();
        }
        if (!m_ofFile.good())
        {
            return E_FAIL;
        }
        return S_OK;
    }

    STDMETHODIMP OutFileStream::SetSize(UINT64 newSize)
    {
        fstream f;
        return S_OK;
    }

}