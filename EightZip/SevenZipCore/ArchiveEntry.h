#ifndef SEVENZIPCORE_ARCHIVEENTRY_H
#define SEVENZIPCORE_ARCHIVEENTRY_H

#include <memory>

#include <boost/optional.hpp>

#include "ComPtr.h"
#include "Platform.h"
#include "TString.h"

namespace SevenZipCore
{
    class Archive;
    class ArchiveFolder;
    class Codecs;
    struct IArchiveOpenCallback;
    struct IInArchive;
    struct IInStream;

    // CArc
    class ArchiveEntry
    {
    public:
        ArchiveEntry(Archive &archive,
            std::shared_ptr<Codecs> cpCodecs,
            TString tstrPath,
            std::shared_ptr<IInStream> cpStream,
            int nSubFileIndex,
            IArchiveOpenCallback *pCallback);
        virtual ~ArchiveEntry();

        inline Archive &GetArchive() const { return m_archive; }

        inline int GetSubfileIndex() const { return m_nSubfileIndex; }
        inline void SetSubfileIndex(int value) { m_nSubfileIndex = value; }

        inline IInArchive &GetInArchive() const { return *m_cpInArchive; }

        inline ArchiveFolder &GetRootFolder() { return *m_upRootFolder; }

    private:
        static const UINT64 MAX_CHECK_START_POSITION = 1 << 22;

        std::shared_ptr<Codecs> m_cpCodecs;

        TString m_tstrPath;
        std::shared_ptr<IInStream> m_cpInStream;
        int m_nSubfileIndex = -1;

        Archive &m_archive;
        SevenZipCore::unique_com_ptr<IInArchive> m_cpInArchive;
        std::unique_ptr<ArchiveFolder> m_upRootFolder;

        void __OpenStream(IArchiveOpenCallback *pCallback);
        void __Close();

        void __LoadFolder();
    };
}

#endif
