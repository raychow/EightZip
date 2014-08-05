// FileStream.h

#ifndef SEVENZIPCORE_FILESTREAM_H
#define SEVENZIPCORE_FILESTREAM_H

#include "COM.h"
#include "IOFile.h"
#include "IStream.h"
#include "IStreamAdapter.h"
#include "TString.h"

#ifndef _SH_DENYRW
#define _SH_DENYRW      0x10    /* deny read/write mode */
#define _SH_DENYWR      0x20    /* deny write mode */
#define _SH_DENYRD      0x30    /* deny read mode */
#define _SH_DENYNO      0x40    /* deny none mode */
#define _SH_SECURE      0x80    /* secure mode */
#endif

namespace SevenZipCore
{
    class InFileStream
        : public IInStream
        , public IStreamGetSize
    {
    public:
        InFileStream() {}
        InFileStream(const TString &tstrPath, bool isShareWrite)
        {
            Open(tstrPath, isShareWrite);
        }
        virtual ~InFileStream() {}

        void Open(const TString &tstrPath, bool isShareWrite);

        STDMETHOD(Read)(void *pData, UINT32 unSize, UINT32 *punProcessedSize);
        STDMETHOD(Seek)(
            INT64 n64Offset, UINT32 unSeekOrigin, UINT64 *pun64NewPosition);

        STDMETHOD(GetSize)(UINT64 *pun64Size);

        IUNKNOWN_IMP2(IInStream, IStreamGetSize)

    protected:
        InFile m_file;

    };

    DECLARE_ADAPTER_CLASS2(
        InFileStream,
        DECLARE_IINSTREAM_ADAPTER,
        DECLARE_ISTREAMGETSIZE_ADAPTER)

    class OutFileStream
        : public IOutStream
    {
    public:
        OutFileStream() {}
        OutFileStream(const TString &tstrPath, bool isTruncate)
        {
            Open(tstrPath, isTruncate);
        }
        virtual ~OutFileStream() {}

        void Open(const TString &tstrPath, bool isTruncate);

        STDMETHOD(Write)(
            const void *pData, UINT32 unSize, UINT32 *punProcessedSize);

        STDMETHOD(Seek)(
            INT64 n64Offset, UINT32 unSeekOrigin, UINT64 *pun64NewPosition);
        STDMETHOD(SetSize)(UINT64 un64Size);

        void SetTime(
            const FILETIME *pftCreated,
            const FILETIME *pftAccessed,
            const FILETIME *pftModified);

        UINT64 GetProcessedSize() const { return m_un64ProcessedSize; }

        IUNKNOWN_IMP1(IOutStream)

    protected:
        OutFile m_file;

        UINT64 m_un64ProcessedSize = 0;

    };

    DECLARE_ADAPTER_CLASS1(
        OutFileStream,
        DECLARE_IOUTSTREAM_ADAPTER)
}

#endif // SEVENZIPCORE_FILESTREAM_H
