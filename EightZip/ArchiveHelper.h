// ArchiveHelper.h

#ifndef ARCHIVEHELPER_H
#define ARCHIVEHELPER_H

#include <memory>

#include "SevenZipCore/TString.h"

#include "ArchiveModel.h"

namespace Helper
{
    bool Extract(TString tstrPath,
        std::shared_ptr<ArchiveModel> spModel,
        bool isLaunchFolder);
}

#endif // ARCHIVEHELPER_H
