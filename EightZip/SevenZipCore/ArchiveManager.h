// ArchiveManager.h

#ifndef ARCHIVEMANAGER_H
#define ARCHIVEMANAGER_H

#include <memory>

#include "Archive.h"
#include "Codecs.h"
#include "ComPtr.h"
#include "IArchive.h"
#include "TString.h"

namespace SevenZipCore
{
    class ArchiveManager
    {
    public:
        ArchiveManager(const TString &tstrBaseFolder);
        ArchiveManager(std::shared_ptr<Codecs> cpCodecs);
        virtual ~ArchiveManager();

        void Open(TString tstrPath, std::shared_ptr<IArchiveOpenCallback> cpCallback);
        void Close();
    private:
        std::shared_ptr<Codecs> m_cpCodecs;
        std::unique_ptr<Archive> m_upArchive;
    };
}

#endif // ARCHIVEMANAGER_H
