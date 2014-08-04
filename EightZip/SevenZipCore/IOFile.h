// IOFile.h

#ifndef SEVENZIPCORE_IOFILE_H
#define SEVENZIPCORE_IOFILE_H

#include "Platform.h"
#include "SmartPointer.h"
#include "TString.h"

namespace SevenZipCore
{
    class IOFile
    {
    public:
        enum struct SeekOrigin
        {
            Begin,
            Current,
            End
        };

        IOFile() {}
        IOFile(TString tstrPath, bool isInput) { Open(tstrPath, isInput); }
        virtual ~IOFile() { Close(); }

        void Open(TString tstrPath, bool isInput);
        void Close() { m_upFile.reset(); }

        UINT64 GetPosition() const;
        UINT64 GetLength() const;

        INT64 Seek(INT64 n64Offset, SeekOrigin seekOrigin) const;

    protected:
#ifdef __WINDOWS__
        HandleUniquePtr m_upFile;
#else
        FileUniquePtr m_upFile;
#endif

    };
}

#endif // SEVENZIPCORE_IOFILE_H