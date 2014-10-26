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
    , m_tstrInternalPath(
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

VirtualModel::EntryVector VirtualModel::_InitializeEntries() const
{
    EntryVector result;
    auto tstrPath = GetPath();
    for (const auto &folder : __GetArchiveFolder()->GetFolders())
    {
        result.push_back(make_shared<VirtualEntry>(
            tstrPath, folder->GetName(), true,
            const_pointer_cast<VirtualModel>(shared_from_this()),
            folder));
    }
    for (const auto &file : __GetArchiveFolder()->GetFiles())
    {
        result.push_back(make_shared<VirtualEntry>(
            tstrPath, file->GetName(), false,
            const_pointer_cast<VirtualModel>(shared_from_this()),
            file));
    }
    return result;
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

void VirtualModel::Extract(TString tstrPath
    , SevenZipCore::IExtractIndicator *pExtractIndicator) const
{
    if (m_spParent)
    {
        vector<UINT32> vun32ArchiveIndex;
        queue<const SevenZipCore::ArchiveFolder *> qpFolder;
        qpFolder.push(m_spArchiveFolder.get());

        while (!qpFolder.empty())
        {
            const auto &folder = *qpFolder.front();
            qpFolder.pop();
            for (const auto &spFile : folder.GetFiles())
            {
                vun32ArchiveIndex.push_back(spFile->GetIndex());
            }
            for (const auto &spFolder : folder.GetFolders())
            {
                auto un32ArchiveIndex = spFolder->GetIndex();
                if (UINT32_MAX != un32ArchiveIndex)
                {
                    vun32ArchiveIndex.push_back(un32ArchiveIndex);
                }
                qpFolder.push(spFolder.get());
            }
        }

        Extract(move(vun32ArchiveIndex), move(tstrPath), pExtractIndicator);
    }
    else
    {
        auto inArchiveAdapter = SevenZipCore::IInArchiveAdapter<>(
            __GetArchiveFolder()->GetArchiveEntry()->GetInArchive());
        inArchiveAdapter.ExtractAll(
            false,
            __CreateCallback(tstrPath, pExtractIndicator).get());
    }
}

TString VirtualModel::Extract(UINT32 un32ArchiveIndex, TString tstrPath
    , SevenZipCore::IExtractIndicator *pExtractIndicator) const
{
    auto inArchiveAdapter = SevenZipCore::IInArchiveAdapter<>(
        __GetArchiveFolder()->GetArchiveEntry()->GetInArchive());
    auto cpArchiveExtractCallback
        = __CreateCallback(tstrPath, pExtractIndicator);
    inArchiveAdapter.Extract(
        vector<UINT32>(1, un32ArchiveIndex),
        false,
        cpArchiveExtractCallback.get());
    return cpArchiveExtractCallback->GetExtractPath();
}

void VirtualModel::Extract(vector<UINT32> vun32ArchiveIndex,
    TString tstrPath, SevenZipCore::IExtractIndicator *pExtractIndicator) const
{
    auto inArchiveAdapter = SevenZipCore::IInArchiveAdapter<>(
        __GetArchiveFolder()->GetArchiveEntry()->GetInArchive());
    sort(vun32ArchiveIndex.begin(), vun32ArchiveIndex.end());
    inArchiveAdapter.Extract(
        vun32ArchiveIndex,
        false,
        __CreateCallback(tstrPath, pExtractIndicator).get());
}

std::shared_ptr<SevenZipCore::Archive> VirtualModel::GetArchive() const
{
    return m_spArchive;
}

const TString &VirtualModel::GetInternalPath() const
{
    return m_tstrInternalPath;
}

shared_ptr<SevenZipCore::ArchiveExtractCallback> VirtualModel::__CreateCallback(
    TString tstrPath, SevenZipCore::IExtractIndicator *pExtractIndicator) const
{
    return SevenZipCore::MakeComPtr(
        new SevenZipCore::ArchiveExtractCallback(
        __GetArchiveFolder()->GetArchiveEntry()->GetArchive(),
        false,
        false,
        false,
        move(tstrPath),
        GetInternalPath(),
        SevenZipCore::ExtractPathMode::CurrentPathNames,
        SevenZipCore::ExtractOverwriteMode::AskBefore,
        pExtractIndicator));
}

std::shared_ptr<SevenZipCore::ArchiveFolder> VirtualModel::__GetArchiveFolder() const
{
    if (!m_spArchiveFolder)
    {
        m_spArchiveFolder = m_spArchive->GetRootFolder();
    }
    return m_spArchiveFolder;
}
