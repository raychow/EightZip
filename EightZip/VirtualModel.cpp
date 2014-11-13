#include "stdwx.h"
#include "VirtualModel.h"

#include <queue>

#include <boost/filesystem.hpp>

#include "SevenZipCore/Archive.h"
#include "SevenZipCore/ArchiveExtractCallback.h"
#include "SevenZipCore/ArchiveFile.h"
#include "SevenZipCore/CommonHelper.h"
#include "SevenZipCore/ComPtr.h"
#include "SevenZipCore/IArchive.h"
#include "SevenZipCore/IArchiveAdapter.h"
#include "SevenZipCore/IExtractIndicator.h"
#include "SevenZipCore/OpenCallback.h"

#include "CodecsLoader.h"
#include "Exception.h"
#include "FileHelper.h"
#include "FileInfo.h"
#include "FolderModel.h"
#include "TempFolder.h"
#include "VirtualEntry.h"

using namespace std;

VirtualModel::VirtualModel(
    TString tstrLocation,
    TString tstrName,
    shared_ptr<ModelBase> spParent,
    shared_ptr<SevenZipCore::IInStream> cpStream,
    shared_ptr<SevenZipCore::IArchiveOpenCallback> cpCallback)
    : ModelBase(move(tstrLocation), move(tstrName))
    , m_spParent(move(spParent))
    , m_spArchive(make_shared<SevenZipCore::Archive>(
    CodecsLoader::GetInstance().GetCodecs()))
{
    m_spArchive->Open(tstrLocation, move(cpStream), move(cpCallback));
}

VirtualModel::VirtualModel(
    TString tstrLocation,
    TString tstrName,
    TString tstrRealPath,
    shared_ptr<ModelBase> spParent,
    shared_ptr<SevenZipCore::IArchiveOpenCallback> cpCallback)
    : ModelBase(move(tstrLocation), move(tstrName))
    , m_spParent(move(spParent))
    , m_spArchive(make_shared<SevenZipCore::Archive>(
    CodecsLoader::GetInstance().GetCodecs()))
{
    m_spArchive->Open(move(tstrRealPath), move(cpCallback));
}

VirtualModel::VirtualModel(
    TString tstrLocation,
    TString tstrInternalLocation,
    TString tstrName,
    shared_ptr<ModelBase> spParent,
    shared_ptr<SevenZipCore::ArchiveFolder> spArchiveFolder)
    : ModelBase(move(tstrLocation), move(tstrName))
    , m_spParent(move(spParent))
    , m_spArchiveFolder(move(spArchiveFolder))
    , m_spArchive(m_spArchiveFolder->GetArchiveEntry()->GetArchive())
    , m_tstrInternalLocation(
    SevenZipCore::Helper::MakePathSlash(move(tstrInternalLocation)))
{
    
}

shared_ptr<ModelBase> VirtualModel::GetParent() const
{
    if (m_spParent)
    {
        return m_spParent;
    }
    else
    {
        return GetModelFromPath(GetLocation());
    }
}

const vector<EntryItemType> &VirtualModel::GetSupportedItems() const
{
    static vector<EntryItemType> vType = {
        EntryItemType::Name,
        EntryItemType::Type,
        EntryItemType::Size,
        EntryItemType::PackedSize,
        //EntryItemType::Modified,
        EntryItemType::CRC
    };
    return vType;
}

VirtualModel::EntryVector VirtualModel::_InitializeEntries() const
{
    EntryVector result;
    auto tstrPath = GetPath();
    for (const auto &folder : GetArchiveFolder()->GetFolders())
    {
        result.push_back(make_shared<VirtualEntry>(
            tstrPath, folder->GetName(), true,
            const_pointer_cast<VirtualModel>(shared_from_this()),
            folder));
    }
    for (const auto &file : GetArchiveFolder()->GetFiles())
    {
        result.push_back(make_shared<VirtualEntry>(
            tstrPath, file->GetName(), false,
            const_pointer_cast<VirtualModel>(shared_from_this()),
            file));
    }
    return result;
}

shared_ptr<SevenZipCore::ArchiveExtractCallback> VirtualModel::__CreateCallback(
    TString tstrPath, SevenZipCore::IExtractIndicator *pExtractIndicator) const
{
    return SevenZipCore::MakeComPtr(
        new SevenZipCore::ArchiveExtractCallback(
        GetArchiveFolder()->GetArchiveEntry()->GetArchive(),
        false,
        false,
        false,
        move(tstrPath),
        GetInternalLocation(),
        SevenZipCore::ExtractPathMode::CurrentPathNames,
        SevenZipCore::ExtractOverwriteMode::AskBefore,
        pExtractIndicator));
}
