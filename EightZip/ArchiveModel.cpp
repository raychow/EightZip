#include "stdwx.h"
#include "ArchiveModel.h"

#include <boost/filesystem.hpp>

#include "SevenZipCore/Archive.h"
#include "SevenZipCore/ArchiveExtractCallback.h"
#include "SevenZipCore/ArchiveFile.h"
#include "SevenZipCore/CommonHelper.h"
#include "SevenZipCore/ComPtr.h"
#include "SevenZipCore/IArchive.h"
#include "SevenZipCore/IArchiveAdapter.h"
#include "SevenZipCore/OpenCallback.h"

#include "CodecsLoader.h"
#include "Exception.h"
#include "FileInfo.h"
#include "Helper.h"
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
        result->LoadChildren();
        return result;
    }
    else
    {
        auto spArchiveEntry = m_spArchiveFile->GetArchiveEntry();
        auto inArchiveAdapter = SevenZipCore::IInArchiveAdapter(
            spArchiveEntry->GetInArchive());
        auto cpGetStream = inArchiveAdapter.QueryInterface<
            SevenZipCore::IInArchiveGetStream>(SevenZipCore::IID_IInArchiveGetStream);
        try
        {
            if (cpGetStream)
            {
                auto cpSubSeqStream = SevenZipCore::IInArchiveGetStreamAdapter
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
                        result->LoadChildren();
                        return result;
                    }
                }
            }
        }
        catch (const SevenZipCore::SevenZipCoreException &)
        {
        }
        __Extract();
        auto result = make_shared<ArchiveModel>(
            m_wpParent.lock(),
            GetPath(),
            TString(),
            m_upTempFolder->GetFilePath(),
            SevenZipCore::MakeComPtr(new SevenZipCore::OpenCallback));
        result->LoadChildren();
        return result;
    }
}

void ArchiveEntry::OpenExternal()
{
    if (!m_upTempFolder)
    {
        __Extract();
    }
    Helper::OpenFileExternal(m_upTempFolder->GetFilePath());
}

void ArchiveEntry::__Extract()
{
    m_upTempFolder.reset(new TempFolder());
    auto spArchiveEntry = m_spArchiveFile->GetArchiveEntry();
    auto spParent = m_wpParent.lock();
    auto inArchiveAdapter = SevenZipCore::IInArchiveAdapter(
        spArchiveEntry->GetInArchive());
    auto cpArchiveExtractCallback = SevenZipCore::MakeComPtr(
        new SevenZipCore::ArchiveExtractCallback(
        spArchiveEntry->GetArchive(),
        false,
        false,
        false,
        m_upTempFolder->GetFolderPath(),
        spParent->GetInternalPath(),
        SevenZipCore::ExtractPathMode::CurrentPathNames,
        SevenZipCore::ExtractOverwriteMode::AskBefore));
    inArchiveAdapter.Extract(
        vector<UINT>(1, m_spArchiveFile->GetIndex()),
        false,
        cpArchiveExtractCallback.get());
    m_upTempFolder->SetFilePath(cpArchiveExtractCallback->GetExtractPath());
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
    m_tstrPath = SevenZipCore::Helper::MakePathSlash(move(tstrVirtualPath));
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
    m_tstrPath = SevenZipCore::Helper::MakePathSlash(tstrVirtualPath);
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
    m_tstrPath = SevenZipCore::Helper::MakePathSlash(move(tstrPath));
}

shared_ptr<IModel> ArchiveModel::GetParent() const
{
    if (m_spParent)
    {
        return m_spParent;
    }
    else
    {
        auto tstrParent = SevenZipCore::Helper::RemovePathSlash(m_tstrPath);
        return make_shared<FolderModel>(
            tstrParent.substr(0, tstrParent.rfind(wxFILE_SEP_PATH) + 1));
    }
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

void ArchiveModel::LoadChildren()
{
    m_vspEntry.clear();
    for (const auto &folder : m_spArchiveFolder->GetFolders())
    {
        m_vspEntry.push_back(make_shared<ArchiveEntry>(
            shared_from_this(),
            folder,
            m_tstrPath,
            true));
    }
    for (const auto &file : m_spArchiveFolder->GetFiles())
    {
        m_vspEntry.push_back(make_shared<ArchiveEntry>(
            shared_from_this(),
            file,
            m_tstrPath,
            false));
    }
}
