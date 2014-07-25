// Archive.h

#ifndef ARCHIVE_H
#define ARCHIVE_H

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
        Archive(std::shared_ptr<Codecs> cpCodecs, TString tstrPath, std::shared_ptr<IArchiveOpenCallback> cpCallback);
        virtual ~Archive();

        void Open(TString tstrPath, std::shared_ptr<IArchiveOpenCallback> cpCallback);
        void Close();

        std::shared_ptr<ArchiveFolder> GetRootFolder() const { return m_spRootFolder; }

    private:
        std::shared_ptr<Codecs> m_cpCodecs;

        TString m_tstrPath;

        std::vector<std::unique_ptr<ArchiveEntry>> m_vupArchiveEntry;
        std::shared_ptr<ArchiveFolder> m_spRootFolder;

        void __LoadArchiveList();
    };
}

#endif // ARCHIVE_H