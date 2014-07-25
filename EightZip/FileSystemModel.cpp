#include "stdwx.h"
#include "FileSystemModel.h"

#include "EightDirFinder.h"
#include "FileInfo.h"

using namespace std;

FileSystemModel::FileSystemModel(TString tstrParentFolder, TString tstrFileName)
{
    m_upPrivate->ParentFolder = tstrParentFolder;
    m_upPrivate->FileName = tstrFileName;
    m_upPrivate->IsDirectory = true;
    // TODO: 20140726
}

const vector<IModel::ItemType> &FileSystemModel::GetChildrenSupportedItems() const
{
    return m_vType;
}

TString FileSystemModel::GetItem(ItemType itemType) const
{
    _UpdateInfo();
    switch (itemType)
    {
    case IModel::ItemType::Name:
        return m_upPrivate->FileName;
    case IModel::ItemType::Size:
        return ToTString(m_upPrivate->Size);
    case IModel::ItemType::Type:
        return FileInfoManager::GetFileInfo(IsDirectory(), GetParentFolder()).GetTypeName();
    case IModel::ItemType::Modified:
        return m_upPrivate->Modified.FormatISOCombined(' ').ToStdWstring();
    case IModel::ItemType::Created:
        return m_upPrivate->Created.FormatISOCombined(' ').ToStdWstring();
    case IModel::ItemType::Accessed:
        return m_upPrivate->Accessed.FormatISOCombined(' ').ToStdWstring();
    case IModel::ItemType::Attributes:
        break;
    default:
        break;
    }
    return wxT("");
}

void FileSystemModel::_UpdateInfo() const
{
    if (m_upPrivate->IsValid)
    {
        return;
    }
    //wxFileName info(m_upPrivate->FullPath);
    //info.Normalize(wxPATH_NORM_ALL & ~wxPATH_NORM_ENV_VARS);
    m_upPrivate->IsValid = true;
}

void FileSystemModel::_UpdateChildren() const
{
    _UpdateInfo();
    if (!m_upPrivate->IsChildrenValid)
    {
        m_upPrivate->Children.clear();
        if (IsDirectory())
        {
            wxFileName dir(GetParentFolder());
            TString tstrParentFolder = GetFullPath();
            EightDirFinder finder(tstrParentFolder);

            while (finder.GetNext())
            {
                auto spModel = make_shared<FileSystemModel>(tstrParentFolder, finder.GetFileName());
                spModel->m_upPrivate->IsDirectory = finder.IsDirectory();
                spModel->m_upPrivate->Size = finder.GetFileSize();
                spModel->m_upPrivate->Accessed = finder.GetAccessed();
                spModel->m_upPrivate->Modified = finder.GetModified();
                spModel->m_upPrivate->Created = finder.GetCreated();
                m_upPrivate->Children.push_back(spModel);
            }
        }
        else
        {
            throw "not implemented yet";
        }
        m_upPrivate->IsChildrenValid = true;
    }
}

std::vector<IModel::ItemType> FileSystemModel::m_vType = {
    IModel::ItemType::Name,
    IModel::ItemType::Size,
    IModel::ItemType::Type,
    IModel::ItemType::Modified,
    IModel::ItemType::Created,
    IModel::ItemType::Accessed,
};
