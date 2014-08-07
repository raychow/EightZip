#include "stdwx.h"
#include "ArchiveModel.h"

#include <boost/filesystem.hpp>

#include "SevenZipCore/CommonHelper.h"
#include "SevenZipCore/IArchiveAdapter.h"
#include "SevenZipCore/OpenCallback.h"

#include "CodecsLoader.h"
#include "Exception.h"
#include "FileInfo.h"
#include "FolderModel.h"
#include "TempFolder.h"

using namespace std;

ArchiveEntry::ArchiveEntry(
    weak_ptr<IModel> wpParent,
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
    if (IsDirectory())
    {
        auto result = make_shared<ArchiveModel>(
            m_wpParent.lock(),
            GetPath(),
            dynamic_pointer_cast<SevenZipCore::ArchiveFolder>(m_spArchiveFile));
        result->LoadChildren();
        return result;
    }
    else
    {
        m_tempFolder.Create(m_tstrName);
        auto cpGetStream = SevenZipCore::IInArchiveAdapter(
            m_spArchiveFile->GetArchiveEntry()->GetArchive()).QueryInterface<
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
                            cpSubStream,
                            MakeComPtr(new SevenZipCore::OpenCallback));
                        result->LoadChildren();
                        return result;
                    }
                }
            }
        }
        catch (const SevenZipCore::SevenZipCoreException &)
        {

        }
    }
}

void ArchiveEntry::OpenExternal() const
{

}

void ArchiveEntry::Open()
{
    //m_tempFolder.Create(m_tstrName);
    //auto cpGetStream
    //    = m_spArchiveFile->GetArchiveEntry()->GetArchiveGetStream();
    //try
    //{
    //    if (cpGetStream)
    //    {
    //        auto cpSubStream = SevenZipCore::IInArchiveGetStreamAdapter
    //            (cpGetStream).GetStream(m_spArchiveFile->GetIndex());
    //        if (cpSubStream)
    //        {
    //            auto result = make_shared<ArchiveModel>(
    //                m_wpParent.lock(),
    //                m_tstrFolder,
    //                m_tempFolder.GetFolderPath(),
    //                nullptr);
    //            result->LoadChildren();

    //        }
    //    }
    //}
    //catch (const SevenZipCore::SevenZipCoreException &)
    //{

    //}
}

ArchiveModel::ArchiveModel(
    shared_ptr<IModel> spParent,
    TString tstrVirtualPath,
    shared_ptr<SevenZipCore::IInStream> cpStream,
    shared_ptr<SevenZipCore::IArchiveOpenCallback> cpCallback)
    : m_upArchive(new SevenZipCore::Archive(
    CodecsLoader::GetInstance().GetCodecs(),
    tstrVirtualPath,
    move(cpStream),
    move(cpCallback)))
    , m_spParent(move(spParent))
{
    m_spArchiveFolder = m_upArchive->GetRootFolder();
    m_tstrPath = move(tstrVirtualPath);
    m_tstrPath.push_back(wxFILE_SEP_PATH);
}

ArchiveModel::ArchiveModel(
    shared_ptr<IModel> spParent,
    TString tstrVirtualPath,
    TString tstrRealPath,
    shared_ptr<SevenZipCore::IArchiveOpenCallback> cpCallback)
    : m_upArchive(new SevenZipCore::Archive(
    CodecsLoader::GetInstance().GetCodecs(),
    move(tstrRealPath),
    move(cpCallback)))
    , m_spParent(move(spParent))
{
    m_spArchiveFolder = m_upArchive->GetRootFolder();
    m_tstrPath = tstrVirtualPath;
}

ArchiveModel::ArchiveModel(
    shared_ptr<IModel> spParent,
    TString tstrPath,
    shared_ptr<SevenZipCore::ArchiveFolder> spArchiveFolder)
    : m_spParent(move(spParent))
    , m_spArchiveFolder(move(spArchiveFolder))
{
    m_tstrPath = tstrPath;
}

shared_ptr<IModel> ArchiveModel::GetParent() const
{
    if (m_spParent)
    {
        return m_spParent;
    }
    else
    {
        auto tstrParent = m_tstrPath;
        tstrParent.pop_back();
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
