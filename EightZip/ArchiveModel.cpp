#include "stdwx.h"
#include "ArchiveModel.h"

#include "FileInfo.h"
#include "FolderModel.h"

using namespace std;

ArchiveEntry::ArchiveEntry(weak_ptr<IModel> wpParent, shared_ptr<SevenZipCore::ArchiveFile> spArchiveFile, TString tstrPath, TString tstrInternalPath, bool isDirectory)
    : m_wpParent(move(wpParent))
    , m_spArchiveFile(move(spArchiveFile))
    , m_tstrInternalPath(move(tstrInternalPath))
{
    m_tstrPath = move(tstrPath);
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
        m_nIconIndex = FileInfo::GetIconIndex(GetFullPath(), IsDirectory(), true);
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
    catch (const std::exception &)
    {
    }
    return wxT("");
}

std::shared_ptr<IModel> ArchiveEntry::GetModel() const
{
    if (IsDirectory())
    {
        auto result = make_shared<ArchiveModel>(m_wpParent.lock(), GetFullPath() + wxFILE_SEP_PATH, m_tstrInternalPath + GetName() + wxFILE_SEP_PATH, dynamic_pointer_cast<SevenZipCore::ArchiveFolder>(m_spArchiveFile));
        result->LoadChildren();
        return result;
    }
    else
    {
        throw "Not implement yet.";
    }
}

void ArchiveEntry::OpenExternal() const
{

}

ArchiveModel::ArchiveModel(std::shared_ptr<IModel> spParent, TString tstrPath, TString tstrInternalPath, shared_ptr<SevenZipCore::Codecs> cpCodecs, TString tstrTempFullPath, std::shared_ptr<SevenZipCore::IArchiveOpenCallback> cpCallback)
    : m_upArchive(new SevenZipCore::Archive(move(cpCodecs), move(tstrTempFullPath), move(cpCallback)))
    , m_spParent(move(spParent))
    , m_tstrInternalPath(move(tstrInternalPath))
{
    m_spArchiveFolder = m_upArchive->GetRootFolder();
    m_tstrPath = tstrPath;
}

ArchiveModel::ArchiveModel(std::shared_ptr<IModel> spParent, TString tstrPath, TString tstrInternalPath, shared_ptr<SevenZipCore::ArchiveFolder> spArchiveFolder)
    : m_spParent(move(spParent))
    , m_spArchiveFolder(move(spArchiveFolder))
    , m_tstrInternalPath(move(tstrInternalPath))
{
    m_tstrPath = tstrPath;
}

std::shared_ptr<IModel> ArchiveModel::GetParent() const
{
    if (m_spParent)
    {
        return m_spParent;
    }
    else
    {
        auto tstrParent = m_tstrPath;
        tstrParent.pop_back();
        return make_shared<FolderModel>(tstrParent.substr(0, tstrParent.rfind(wxFILE_SEP_PATH) + 1));
    }
}

const std::vector<IEntry::ItemType> & ArchiveModel::GetSupportedItems() const
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
        m_vspEntry.push_back(make_shared<ArchiveEntry>(shared_from_this()
            , folder
            , m_tstrPath
            , m_tstrInternalPath
            , true));
    }
    for (const auto &file : m_spArchiveFolder->GetFiles())
    {
        m_vspEntry.push_back(make_shared<ArchiveEntry>(shared_from_this()
            , file
            , m_tstrPath
            , m_tstrInternalPath
            , false));
    }
}
