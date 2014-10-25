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

shared_ptr<IModel> ArchiveEntry::GetModel() const
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

vector<UINT32> ArchiveEntry::GetAllArchiveIndexes() const
{
    vector<UINT32> result = { GetArchiveIndex() };
    if (IsDirectory())
    {
        auto indexes = dynamic_pointer_cast<ArchiveModel>(
            GetModel())->GetArchiveIndexes();
        result.insert(result.end(), indexes.cbegin(), indexes.cend());
    }
    return result;
}

void ArchiveEntry::OpenExternal() const
{
    if (!m_upTempFolder)
    {
        __ExtractToTempFolder();
    }
    Helper::OpenFileExternal(m_upTempFolder->GetFilePath());
}

void ArchiveEntry::__ExtractToTempFolder() const
{
    m_upTempFolder.reset(new TempFolder());
    m_upTempFolder->SetFilePath(dynamic_pointer_cast<ArchiveModel>(
        m_wpParent.lock())->Extract(GetArchiveIndex(),
        m_upTempFolder->GetFolderPath(), nullptr));
}

ArchiveModel::ArchiveModel(
    shared_ptr<ModelBase> spParent,
    TString tstrVirtualLocation,
    TString tstrVirtualFileName,
    TString tstrInternalPath,
    shared_ptr<SevenZipCore::IInStream> cpStream)
    : ModelBase(tstrVirtualLocation, tstrVirtualFileName, false, true)
    , m_spParent(move(spParent))
    , m_spArchive(make_shared<SevenZipCore::Archive>(
    CodecsLoader::GetInstance().GetCodecs()))
    , m_tstrInternalPath(
    SevenZipCore::Helper::MakePathSlash(move(tstrInternalPath)))
{
    m_spArchive->Open(GetPath(), move(cpStream), nullptr/* TODO */);
}

ArchiveModel::ArchiveModel(
    shared_ptr<ModelBase> spParent,
    TString tstrVirtualLocation,
    TString tstrVirtualFileName,
    TString tstrInternalPath,
    TString tstrArchivePath)
    : ModelBase(tstrVirtualLocation, tstrVirtualFileName, false, true)
    , m_spParent(move(spParent))
    , m_spArchive(make_shared<SevenZipCore::Archive>(
    CodecsLoader::GetInstance().GetCodecs()))
    , m_tstrInternalPath(
    SevenZipCore::Helper::MakePathSlash(move(tstrInternalPath)))
{
    m_spArchive->Open(move(tstrArchivePath), nullptr/* TODO */);
}

ArchiveModel::ArchiveModel(
    shared_ptr<ModelBase> spParent,
    TString tstrVirtualLocation,
    TString tstrVirtualFileName,
    TString tstrInternalPath,
    shared_ptr<SevenZipCore::ArchiveFolder> spArchiveFolder)
    : ModelBase(tstrVirtualLocation, tstrVirtualFileName, true, true)
    , m_spParent(move(spParent))
    , m_spArchiveFile(move(spArchiveFolder))
    , m_spArchive(m_spArchiveFile->GetArchiveEntry()->GetArchive())
    , m_tstrInternalPath(
    SevenZipCore::Helper::MakePathSlash(move(tstrInternalPath)))
{

}

shared_ptr<ModelBase> ArchiveModel::GetParent() const
{
    return m_spParent;
}

const vector<ModelItemType> &ArchiveModel::GetSupportedItems() const
{
    static vector<ModelItemType> vType = {
        ModelItemType::Name,
        ModelItemType::Type,
        ModelItemType::Size,
        ModelItemType::PackedSize,
        //ModelItemType::Modified,
        ModelItemType::CRC
    };
    return vType;
}

bool ArchiveModel::CanExtract() const
{
    return true;
}

void ArchiveModel::Extract(TString tstrPath
    , SevenZipCore::IExtractIndicator *pExtractIndicator) const
{
    if (m_spParent)
    {
        Extract(GetArchiveIndexes(), move(tstrPath), pExtractIndicator);
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

TString ArchiveModel::Extract(UINT32 un32ArchiveIndex, TString tstrPath
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

void ArchiveModel::Extract(vector<UINT32> vun32ArchiveIndex,
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

shared_ptr<SevenZipCore::Archive> ArchiveModel::GetArchive() const
{
    return m_spArchive;
}

vector<UINT32> ArchiveModel::GetArchiveIndexes() const
{
    vector<UINT32> result;
    queue<const SevenZipCore::ArchiveFolder *> qpFolder;
    qpFolder.push(m_spArchiveFile.get());

    while (!qpFolder.empty())
    {
        const auto &folder = *qpFolder.front();
        qpFolder.pop();
        for (const auto &spFile : folder.GetFiles())
        {
            result.push_back(spFile->GetIndex());
        }
        for (const auto &spFolder : folder.GetFolders())
        {
            auto un32ArchiveIndex = spFolder->GetIndex();
            if (UINT32_MAX != un32ArchiveIndex)
            {
                result.push_back(un32ArchiveIndex);
            }
            qpFolder.push(spFolder.get());
        }
    }

    return result;
}

ModelBase::Models ArchiveModel::GetChildren() const
{
    Models result;
    if (IsDirectory())
    {
        for (const auto &folder : m_spArchiveFile->GetFolders())
        {
            result.push_back(make_shared<ArchiveEntry>(
                const_pointer_cast<ArchiveModel>(shared_from_this()),
                folder,
                m_tstrPath,
                true));
        }
        for (const auto &file : __GetArchiveFolder()->GetFiles())
        {
            result.push_back(make_shared<ArchiveEntry>(
                const_pointer_cast<ArchiveModel>(shared_from_this()),
                file,
                m_tstrPath,
                false));
        }
    }

    return result;
}

shared_ptr<SevenZipCore::ArchiveExtractCallback> ArchiveModel::__CreateCallback(
    TString tstrPath, SevenZipCore::IExtractIndicator *pExtractIndicator) const
{
    return SevenZipCore::MakeComPtr(
        new SevenZipCore::ArchiveExtractCallback(
        m_spArchiveFile->GetArchiveEntry()->GetArchive(),
        false,
        false,
        false,
        move(tstrPath),
        GetInternalPath(),
        SevenZipCore::ExtractPathMode::CurrentPathNames,
        SevenZipCore::ExtractOverwriteMode::AskBefore,
        pExtractIndicator));
}
