// Archive.h

#ifndef SEVENZIPCORE_ARCHIVE_H
#define SEVENZIPCORE_ARCHIVE_H

#include <memory>
#include <vector>

#include "ArchiveEntry.h"
#include "ArchiveFile.h"
#include "Codecs.h"
#include "IArchive.h"
#include "TString.h"

namespace SevenZipCore
{
    // CArchiveLink
    class Archive
    {
    public:
        Archive(std::shared_ptr<Codecs> cpCodecs);
        Archive(
            std::shared_ptr<Codecs> cpCodecs,
            TString tstrPath,
            std::shared_ptr<IArchiveOpenCallback> cpCallback);
        virtual ~Archive();

        void Open(
            TString tstrPath,
            std::shared_ptr<IArchiveOpenCallback> cpCallback);
        void Close();

        TString GetPath() const { return m_tstrPath; }
        std::shared_ptr<ArchiveFolder> GetRootFolder() const
        {
            return m_spRootFolder;
        }

        std::shared_ptr<ArchiveEntry> GetArchiveEntry() const;

    private:
        std::shared_ptr<Codecs> m_cpCodecs;

        TString m_tstrPath;

        std::vector<std::shared_ptr<ArchiveEntry>> m_vspArchiveEntry;
        std::shared_ptr<ArchiveFolder> m_spRootFolder;

        void __LoadArchiveList();
    };
}

#endif // SEVENZIPCORE_ARCHIVE_H