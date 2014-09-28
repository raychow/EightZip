#include "stdwx.h"
#include "ArchiveModel.h"

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

using namespace std;

ArchiveEntry::ArchiveEntry(
    weak_ptr<ArchiveModel> wpParent,
    shared_ptr<SevenZipCore::ArchiveFile> spArchiveFile,
    TString tstrPath,
    bool isDirectory)
    : m_wpParent(move(wpParent))
    , m_spArchiveFile(move(spArchiveFile))
{
    m_tstrFolder = SevenZipCore::Helper::MakePathSlash(move(tstrPath));
    m_tstrName = m_spArchiveFile->GetName();
    m_isDirectory = isDirectory;
    m_un64Size = m_spArchiveFile->GetSize();
    m_un64PackedSize = m_spArchiveFile->GetPackedSize();
    m_tstrType = FileInfo::GetType(m_tstrName, IsDirectory());
    m_oun32CRC = m_spArchiveFile->GetCRC();
}

int ArchiveEntry::GetIconIndex() const
{
    if (-1 == m_nIconIndex)
    {
        m_nIconIndex = FileInfo::GetIconIndex(
            GetPath(), IsDirectory(), true);
    }
    return m_nIconIndex;
}

TString ArchiveEntry::GetItem(ItemType itemType) const
{
    try
    {
        switch (itemType)
        {
        case ItemType::Size:
            return ToTString(m_un64Size);
        case ItemType::PackedSize:
            return ToTString(m_un64PackedSize);
        default:
            return EntryBase::GetItem(itemType);
        }
    }
    catch (const exception &)
    {
    }
    return wxEmptyString;
}

shared_ptr<IModel> ArchiveEntry::GetModel()
{
    auto spParent = m_wpParent.lock();
    if (IsDirectory())
    {
        auto result = make_shared<ArchiveModel>(
            spParent,
            GetPath(),
            spParent->GetInternalPath() + GetName(),
            dynamic_pointer_cast<SevenZipCore::ArchiveFolder>(m_spArchiveFile));
        return result;
    }
    else
    {
        auto spArchiveEntry = m_spArchiveFile->GetArchiveEntry();
        SevenZipCore::IInArchiveAdapter<> inArchiveAdapter(
            spArchiveEntry->GetInArchive());
        try
        {
            auto cpGetStream = inArchiveAdapter.QueryInterface<
                SevenZipCore::IInArchiveGetStream>(SevenZipCore::IID_IInArchiveGetStream);
            if (cpGetStream)
            {
                auto cpSubSeqStream = SevenZipCore::IInArchiveGetStreamAdapter<>
                    (cpGetStream).GetStream(m_spArchiveFile->GetIndex());
                if (cpSubSeqStream)
                {
                    auto cpSubStream = 
                        SevenZipCore::Helper::QueryInterface<
                        SevenZipCore::IInStream>(
                        cpSubSeqStream, SevenZipCore::IID_IInStream);
                    if (cpSubStream)
                    {
                        auto result = make_shared<ArchiveModel>(
                            m_wpParent.lock(),
                            GetPath(),
                            TString(),
                            cpSubStream,
                            SevenZipCore::MakeComPtr(new SevenZipCore::OpenCallback));
                        return result;
                    }
                }
            }
        }
        catch (const SevenZipCore::SevenZipCoreException &)
        {
        }
        __ExtractToTempFolder();
        auto result = make_shared<ArchiveModel>(
            m_wpParent.lock(),
            GetPath(),
            TString(),
            m_upTempFolder->GetFilePath(),
            SevenZipCore::MakeComPtr(new SevenZipCore::OpenCallback));
        return result;
    }
}

UINT32 ArchiveEntry::GetArchiveIndex() const
{
    return m_spArchiveFile->GetIndex();
}

void ArchiveEntry::OpenExternal()
{
    if (!m_upTempFolder)
    {
        __ExtractToTempFolder();
    }
    Helper::OpenFileExternal(m_upTempFolder->GetFilePath());
}

void ArchiveEntry::__ExtractToTempFolder()
{
    m_upTempFolder.reset(new TempFolder());
    m_upTempFolder->SetFilePath(dynamic_pointer_cast<ArchiveModel>(
        m_wpParent.lock())->Extract(GetArchiveIndex(),
        m_upTempFolder->GetFolderPath(), nullptr));
}

ArchiveModel::ArchiveModel(
    shared_ptr<IModel> spParent,
    TString tstrVirtualPath,
    TString tstrInternalPath,
    shared_ptr<SevenZipCore::IInStream> cpStream,
    shared_ptr<SevenZipCore::IArchiveOpenCallback> cpCallback)
    : m_tstrInternalPath(
    SevenZipCore::Helper::MakePathSlash(move(tstrInternalPath)))
    , m_spArchive(make_shared<SevenZipCore::Archive>(
    CodecsLoader::GetInstance().GetCodecs()))
    , m_spParent(move(spParent))
{
    m_tstrPath = SevenZipCore::Helper::RemovePathSlash(move(tstrVirtualPath));
    m_spArchive->Open(tstrVirtualPath, move(cpStream), move(cpCallback));
    m_spArchiveFolder = m_spArchive->GetRootFolder();
}

ArchiveModel::ArchiveModel(
    shared_ptr<IModel> spParent,
    TString tstrVirtualPath,
    TString tstrInternalPath,
    TString tstrRealPath,
    shared_ptr<SevenZipCore::IArchiveOpenCallback> cpCallback)
    : m_tstrInternalPath(
    SevenZipCore::Helper::MakePathSlash(move(tstrInternalPath)))
    , m_spArchive(make_shared<SevenZipCore::Archive>(
    CodecsLoader::GetInstance().GetCodecs()))
    , m_spParent(move(spParent))
{
    m_tstrPath = SevenZipCore::Helper::RemovePathSlash(tstrVirtualPath);
    m_spArchive->Open(move(tstrRealPath), move(cpCallback));
    m_spArchiveFolder = m_spArchive->GetRootFolder();
}

ArchiveModel::ArchiveModel(
    shared_ptr<IModel> spParent,
    TString tstrPath,
    TString tstrInternalPath,
    shared_ptr<SevenZipCore::ArchiveFolder> spArchiveFolder)
    : m_tstrInternalPath(
    SevenZipCore::Helper::MakePathSlash(move(tstrInternalPath)))
    , m_spParent(move(spParent))
    , m_spArchiveFolder(move(spArchiveFolder))
{
    m_tstrPath = SevenZipCore::Helper::RemovePathSlash(move(tstrPath));
}

shared_ptr<IModel> ArchiveModel::GetParent() const
{
    if (m_spParent)
    {
        return m_spParent;
    }
    else
    {
        auto tstrParentPath = m_tstrPath.substr(0,
            m_tstrPath.rfind(wxFILE_SEP_PATH) + 1);
        try
        {
            return make_shared<FolderModel>(tstrParentPath);
        }
        catch (const ModelException &)
        {
            return GetModelFromPath(tstrParentPath);
        }
    }
}

const ArchiveModel::EntryVector &ArchiveModel::GetEntries() const
{
    if (!m_isInitialized)
    {
        m_vspEntry.clear();
        for (const auto &folder : m_spArchiveFolder->GetFolders())
        {
            m_vspEntry.push_back(make_shared<ArchiveEntry>(
                const_pointer_cast<ArchiveModel>(shared_from_this()),
                folder,
                m_tstrPath,
                true));
        }
        for (const auto &file : m_spArchiveFolder->GetFiles())
        {
            m_vspEntry.push_back(make_shared<ArchiveEntry>(
                const_pointer_cast<ArchiveModel>(shared_from_this()),
                file,
                m_tstrPath,
                false));
        }
        m_isInitialized = true;
    }
    return m_vspEntry;
}

const vector<IEntry::ItemType> & ArchiveModel::GetSupportedItems() const
{
    static vector<IEntry::ItemType> vType = {
        IEntry::ItemType::Name,
        IEntry::ItemType::Type,
        IEntry::ItemType::Size,
        IEntry::ItemType::PackedSize,
        //IEntry::ItemType::Modified,
        IEntry::ItemType::CRC
    };
    return vType;
}

void ArchiveModel::Extract(TString tstrPath
    , SevenZipCore::IExtractIndicator *pExtractIndicator) const
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

TString ArchiveModel::Extract(UINT32 un32ArchiveIndex, TString tstrPath
    , SevenZipCore::IExtractIndicator *pExtractIndicator) const
{
    auto spArchiveEntry = m_spArchiveFolder->GetArchiveEntry();
    auto inArchiveAdapter = SevenZipCore::IInArchiveAdapter<>(
        spArchiveEntry->GetInArchive());
    auto cpArchiveExtractCallback = SevenZipCore::MakeComPtr(
        new SevenZipCore::ArchiveExtractCallback(
        spArchiveEntry->GetArchive(),
        false,
        false,
        false,
        move(tstrPath),
        GetInternalPath(),
        SevenZipCore::ExtractPathMode::CurrentPathNames,
        SevenZipCore::ExtractOverwriteMode::AskBefore,
        pExtractIndicator));
    inArchiveAdapter.Extract(
        vector<UINT32>(1, un32ArchiveIndex),
        false,
        cpArchiveExtractCallback.get());
    return cpArchiveExtractCallback->GetExtractPath();
}

void ArchiveModel::Extract(vector<UINT32> vun32ArchiveIndex,
    TString tstrPath, SevenZipCore::IExtractIndicator *pExtractIndicator) const
{
    auto spArchiveEntry = m_spArchiveFolder->GetArchiveEntry();
    auto inArchiveAdapter = SevenZipCore::IInArchiveAdapter<>(
        spArchiveEntry->GetInArchive());
    auto cpArchiveExtractCallback = SevenZipCore::MakeComPtr(
        new SevenZipCore::ArchiveExtractCallback(
        spArchiveEntry->GetArchive(),
        false,
        false,
        false,
        move(tstrPath),
        GetInternalPath(),
        SevenZipCore::ExtractPathMode::CurrentPathNames,
        SevenZipCore::ExtractOverwriteMode::AskBefore,
        pExtractIndicator));
    sort(vun32ArchiveIndex.begin(), vun32ArchiveIndex.end());
    inArchiveAdapter.Extract(
        vun32ArchiveIndex,
        false,
        cpArchiveExtractCallback.get());
}

std::shared_ptr<SevenZipCore::Archive> ArchiveModel::GetArchive() const
{
    return m_spArchive;
}

const TString & ArchiveModel::GetInternalPath() const
{
    return m_tstrInternalPath;
}
