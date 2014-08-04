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
        // ReadFile and WriteFile functions in Windows have BUG:
        // If you Read or Write 64MB or more
        // (probably min_failure_size = 64MB - 32KB + 1)
        // from/to Network file, it returns ERROR_NO_SYSTEM_RESOURCES
        // (Insufficient system resources exist to complete the requested service).

        // Probably in some version of Windows there are problems with other sizes:
        // for 32 MB (maybe also for 16 MB).
        // And message can be "Network connection was lost"
        const static int MAX_CHUNK_SIZE = 1 << 22;

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

    class InFile
        : public IOFile
    {
    public:
        InFile() {}
        InFile(TString tstrPath) { Open(tstrPath); }
        virtual ~InFile() { Close(); }

        void Open(TString tstrPath)
        {
            IOFile::Open(tstrPath, true);
        }
        void Read(BYTE *pbyBuffer, UINT32 nBytesToRead);
        UINT32 ReadPart(BYTE *pbyBuffer, UINT32 nBytesToRead);

    };

    class OutFile
        : public IOFile
    {
    public:
        OutFile() {}
        OutFile(TString tstrPath) { Open(tstrPath); }
        virtual ~OutFile() { Close(); }

        void Open(TString tstrPath)
        {
            IOFile::Open(tstrPath, false);
        }

        void Write(BYTE *pbyBuffer, UINT32 nBytesToWrite);
        UINT32 WritePart(BYTE *pbyBuffer, UINT32 nBytesToWrite);
        void SetLength(UINT64 un64Length);
    };
}

#endif // SEVENZIPCORE_IOFILE_H