#include "stdwx.h"
#include "ArchiveModel.h"

#include "FileInfo.h"
#include "FolderModel.h"

using namespace std;

ArchiveEntry::ArchiveEntry(shared_ptr<IModel> spParent, shared_ptr<SevenZipCore::ArchiveFile> spArchiveFile, TString tstrPath, bool isDirectory)
    : m_spParent(spParent)
    , m_spArchiveFile(spArchiveFile)
{
    m_tstrPath = move(tstrPath);
    m_tstrName = spArchiveFile->GetName();
    m_isDirectory = isDirectory;
    m_un64Size = spArchiveFile->GetSize();
    m_un64PackedSize = spArchiveFile->GetPackedSize();
    m_tstrType = FileInfo::GetType(m_tstrName, IsDirectory());
    m_oun32CRC = spArchiveFile->GetCRC();
}

std::shared_ptr<IModel> ArchiveEntry::GetModel() const
{
    if (IsDirectory())
    {
        auto result = make_shared<ArchiveModel>(m_spParent, GetFullPath(), dynamic_pointer_cast<SevenZipCore::ArchiveFolder>(m_spArchiveFile));
        result->LoadChildren();
        return result;
    }
    else
    {
        throw "Not implement yet.";
    }
}

int ArchiveEntry::GetIconIndex() const
{
    if (-1 == m_nIconIndex)
    {
        m_nIconIndex = FileInfo::GetIconIndex(GetFullPath(), IsDirectory(), true);
    }
    return m_nIconIndex;
}

ArchiveModel::ArchiveModel(std::shared_ptr<IModel> spParent, TString tstrPath, shared_ptr<SevenZipCore::Codecs> cpCodecs, TString tstrTempFullPath, std::shared_ptr<SevenZipCore::IArchiveOpenCallback> cpCallback)
    : ArchiveModel(spParent, tstrPath, SevenZipCore::Archive(move(cpCodecs), tstrTempFullPath, move(cpCallback)).GetRootFolder())
{

}

ArchiveModel::ArchiveModel(std::shared_ptr<IModel> spParent, TString tstrPath, shared_ptr<SevenZipCore::ArchiveFolder> spArchiveFolder)
    : m_spParent(move(spParent))
    , m_spArchiveFolder(move(spArchiveFolder))
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
            , true));
    }
    for (const auto &file : m_spArchiveFolder->GetFiles())
    {
        m_vspEntry.push_back(make_shared<ArchiveEntry>(shared_from_this()
            , file
            , m_tstrPath
            , false));
    }
}
