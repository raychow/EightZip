// ArchiveHelper.h

#ifndef ARCHIVEHELPER_H
#define ARCHIVEHELPER_H

#include <memory>

#include "SevenZipCore/TString.h"

class VirtualModel;

namespace Helper
{
    bool Extract(TString tstrPath,
        std::shared_ptr<VirtualModel> spModel,
        bool isLaunchFolder);
}

#endif // ARCHIVEHELPER_H
