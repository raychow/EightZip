#include "stdwx.h"
#include "VirtualRootModel.h"

using namespace std;

#include "SevenZipCore/Archive.h"
#include "SevenZipCore/OpenCallback.h"

#include "ArchiveHelper.h"
#include "OpenIndicator.h"

ArchiveHandler::ArchiveHandler(TString tstrPath,
    ProgressDialog *pProgressDialog)
{
    auto openIndicator = OpenIndicator { &GetProperty(), pProgressDialog };
    auto upCallback = SevenZipCore::MakeUniqueCom(
        new SevenZipCore::OpenCallback { &openIndicator });
    m_upArchive = Helper::CreateArchive(move(tstrPath), upCallback.get());
}

ArchiveHandler::ArchiveHandler(TString tstrPath,
    shared_ptr<SevenZipCore::IInStream> cpStream,
    ProgressDialog *pProgressDialog)
{
    auto openIndicator = OpenIndicator { &GetProperty(), pProgressDialog };
    auto upCallback = SevenZipCore::MakeUniqueCom(
        new SevenZipCore::OpenCallback { &openIndicator });
    m_upArchive = Helper::CreateArchive(
        move(tstrPath), move(cpStream), upCallback.get());
}

VirtualRootModel::VirtualRootModel(TString tstrLocation,
    TString tstrName,
    shared_ptr<VirtualModel> spParent,
    shared_ptr<SevenZipCore::IInStream> cpStream,
    ProgressDialog *pProgressDialog)
    : ArchiveHandler(SevenZipCore::Helper::MakePathSlash(
    tstrLocation) + tstrName, move(cpStream), pProgressDialog)
    , VirtualModel(move(tstrLocation), move(tstrName), move(spParent),
    ArchiveHandler::GetArchive(), ArchiveHandler::GetProperty())
{
}

VirtualRootModel::VirtualRootModel(TString tstrLocation,
    TString tstrName,
    TString tstrRealPath,
    shared_ptr<VirtualModel> spParent,
    ProgressDialog *pProgressDialog)
    : ArchiveHandler(move(tstrRealPath), pProgressDialog)
    , VirtualModel(move(tstrLocation), move(tstrName), spParent,
    ArchiveHandler::GetArchive(), ArchiveHandler::GetProperty())
{
}
