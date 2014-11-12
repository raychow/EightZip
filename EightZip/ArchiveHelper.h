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
        std::shared_ptr<VirtualModel> spModel,
        bool isLaunchFolder);
    bool Extract(TString tstrPath,
        std::vector<std::shared_ptr<EntryBase>> vspEntry,
        bool isLaunchFolder);
}

#endif // ARCHIVEHELPER_H
