// OutFileStream.h

#ifndef SEVENZIPCORE_OUTFILESTREAM_H
#define SEVENZIPCORE_OUTFILESTREAM_H

#include <fstream>

#include "IStream.h"
#include "TString.h"

namespace SevenZipCore
{
    class OutFileStream
        : public IOutStream
    {
    public:
        OutFileStream(TString tstrPath, std::ios::open_mode openMode)
        {
            m_ofFile.open(tstrPath, openMode);
        }
        virtual ~OutFileStream() { Close(); }

        void Close() { m_ofFile.close(); }

        STDMETHOD(Write)(const void *data, UINT32 size, UINT32 *processedSize);
        STDMETHOD(Seek)(
            INT64 offset, UINT32 seekOrigin, UINT64 *newPosition);
        STDMETHOD(SetSize)(UINT64 newSize);

        UINT64 GetPosition() { return m_ofFile.tellp(); }

    private:
        std::ofstream m_ofFile;

    };
}

#endif // SEVENZIPCORE_OUTFILESTREAM_H
