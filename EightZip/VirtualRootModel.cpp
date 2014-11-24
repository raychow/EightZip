#include "stdwx.h"
#include "VirtualRootModel.h"

using namespace std;

#include "CodecsLoader.h"

VirtualRootModel::VirtualRootModel(TString tstrLocation,
    TString tstrName,
    shared_ptr<ModelBase> spParent,
    shared_ptr<SevenZipCore::IInStream> cpStream,
    SevenZipCore::IArchiveOpenCallback *pCallback)
    : ArchiveHandler(make_unique<SevenZipCore::Archive>(
    CodecsLoader::GetInstance().GetCodecs(),
    tstrLocation, move(cpStream), pCallback))
    , VirtualModel(move(tstrLocation), move(tstrName),
    ArchiveHandler::GetArchive())
{
}

VirtualRootModel::VirtualRootModel(TString tstrLocation,
    TString tstrName,
    TString tstrRealPath,
    shared_ptr<ModelBase> spParent,
    SevenZipCore::IArchiveOpenCallback *pCallback)
    : ArchiveHandler(make_unique<SevenZipCore::Archive>(
    CodecsLoader::GetInstance().GetCodecs(), move(tstrRealPath), pCallback))
    , VirtualModel(move(tstrLocation), move(tstrName),
    ArchiveHandler::GetArchive())
{
}
