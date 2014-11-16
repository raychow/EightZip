// ArchiveHelper.h

#ifndef ARCHIVEHELPER_H
#define ARCHIVEHELPER_H

#include <memory>
#include <vector>

#include "SevenZipCore/TString.h"

class EntryBase;
class VirtualModel;

namespace Helper
{
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
