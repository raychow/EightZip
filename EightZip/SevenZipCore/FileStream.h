// FileStream.h

#ifndef SEVENZIPCORE_FILESTREAM_H
#define SEVENZIPCORE_FILESTREAM_H

#include <fstream>

#include "COM.h"
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
        InFileStream() {};
        InFileStream(const TString &tstrPath);
        virtual ~InFileStream();

        void Open(const TString &tstrPath);

        STDMETHOD(Read)(void *data, UINT32 size, UINT32 *processedSize);
        STDMETHOD(Seek)(INT64 offset, UINT32 seekOrigin, UINT64 *newPosition);

        STDMETHOD(GetSize)(UINT64 *size);

        IUNKNOWN_IMP2(IInStream, IStreamGetSize)
    protected:
        std::ifstream m_ifFile;
    };

    DECLARE_ADAPTER_CLASS2(
        InFileStream,
        DECLARE_IINSTREAM_ADAPTER,
        DECLARE_ISTREAMGETSIZE_ADAPTER)
}

#endif // SEVENZIPCORE_FILESTREAM_H
