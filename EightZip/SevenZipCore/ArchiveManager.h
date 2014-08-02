// ArchiveManager.h

#ifndef SEVENZIPCORE_ARCHIVEMANAGER_H
#define SEVENZIPCORE_ARCHIVEMANAGER_H

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

        void Open(
            TString tstrPath, std::shared_ptr<IArchiveOpenCallback> cpCallback);
        void Close();
    private:
        std::shared_ptr<Codecs> m_cpCodecs;
        std::unique_ptr<Archive> m_upArchive;
    };
}

#endif // SEVENZIPCORE_ARCHIVEMANAGER_H
