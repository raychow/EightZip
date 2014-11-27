// ArchiveHelper.h

#ifndef ARCHIVEHELPER_H
#define ARCHIVEHELPER_H

#include <memory>
#include <vector>

#include "SevenZipCore/TString.h"

namespace SevenZipCore
{
    class Archive;
    struct IArchiveOpenCallback;
    struct IInStream;
}

class EntryBase;
class VirtualModel;

namespace Helper
{
    std::unique_ptr<SevenZipCore::Archive> CreateArchive(TString tstrPath,
        SevenZipCore::IArchiveOpenCallback *pCallback);
    std::unique_ptr<SevenZipCore::Archive> CreateArchive(TString tstrPath,
        std::shared_ptr<SevenZipCore::IInStream> cpStream,
        SevenZipCore::IArchiveOpenCallback *pCallback);

    bool Extract(TString tstrPath,
        TString tstrInternalPath,
        std::shared_ptr<VirtualModel> spModel,
        bool isLaunchFolder);
    bool Extract(TString tstrPath,
        TString tstrInternalPath,
        const std::vector<std::reference_wrapper<EntryBase>> &vEntry,
        bool isLaunchFolder);
}

#endif // ARCHIVEHELPER_H
