#include "stdwx.h"
#include "FileSystemModel.h"

#include "FileFinder.h"
#include "FileInfo.h"

using namespace std;

FileSystemModel::FileSystemModel(TString tstrFullPath)
{
    FileInfo fileInfo(tstrFullPath);
    if (fileInfo.IsOK())
    {
        while (wxEndsWithPathSeparator(tstrFullPath))
        {
            tstrFullPath.pop_back();
        }
        wxFileName fileName(tstrFullPath);
        fileName.Normalize();
        m_upPrivate->Path = fileName.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
        m_upPrivate->Name = fileName.GetName();
        m_upPrivate->IsDirectory = fileInfo.IsDirectory();
        m_upPrivate->Size = fileInfo.GetSize();
        m_upPrivate->Accessed = fileInfo.GetAccessed();
        m_upPrivate->Modified = fileInfo.GetModified();
        m_upPrivate->Created = fileInfo.GetCreated();
    }
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
        return m_upPrivate->Name;
    case IModel::ItemType::Size:
        return ToTString(m_upPrivate->Size);
    case IModel::ItemType::Type:
        //return FileInfoManager::GetFileInfo(IsDirectory(), GetPath()).GetTypeName();
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
            wxFileName dir(GetPath());
            TString tstrParentFolder = GetFullPath();
            FileFinder finder(tstrParentFolder);

            while (finder.FindNext())
            {
                auto spModel = __MakeShared();
                spModel->m_upPrivate->Path = tstrParentFolder;
                spModel->m_upPrivate->Name = finder.GetFileName();
                spModel->m_upPrivate->IsDirectory = finder.IsDirectory();
                spModel->m_upPrivate->Size = finder.GetSize();
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

std::shared_ptr<FileSystemModel> FileSystemModel::__MakeShared()
{
    return make_shared<FileSystemModel>(Private());
}
