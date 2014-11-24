#include "stdwx.h"
#include "VirtualModel.h"

#include <queue>

#include <boost/filesystem.hpp>

#include "SevenZipCore/Archive.h"
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

VirtualModel::VirtualModel(TString tstrLocation,
    TString tstrInternalLocation,
    TString tstrName,
    shared_ptr<VirtualModel> spParent,
    SevenZipCore::ArchiveFolder &archiveFolder)
    : ModelBase(move(tstrLocation), move(tstrName))
    , m_archive(spParent->m_archive)
    , m_spParent(move(spParent))
    , m_archiveFolder(archiveFolder)
    , m_tstrInternalLocation(
    SevenZipCore::Helper::MakePathSlash(move(tstrInternalLocation)))
{
}

VirtualModel::VirtualModel(TString tstrLocation,
    TString tstrName,
    SevenZipCore::Archive &archive)
    : ModelBase(move(tstrLocation), move(tstrName))
    , m_archive(archive)
    , m_archiveFolder(archive.GetArchiveEntry().GetRootFolder())
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
    for (const auto &folder : m_archiveFolder.GetFolders())
    {
        result.push_back(make_unique<VirtualEntry>(
            tstrPath, folder->GetName(), true,
            const_pointer_cast<VirtualModel>(shared_from_this()),
            *folder));
    }
    for (const auto &file : m_archiveFolder.GetFiles())
    {
        result.push_back(make_unique<VirtualEntry>(
            tstrPath, file->GetName(), false,
            const_pointer_cast<VirtualModel>(shared_from_this()),
            *file));
    }
    return result;
}
