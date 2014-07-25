#include "stdwx.h"
#include "FileSystemModel.h"

using namespace std;

FileSystemModel::FileSystemModel(TString tstrPath)
{
    m_upPrivate->FullPath = tstrPath;
    _UpdateInfo();
}

const vector<IModel::ItemType> &FileSystemModel::GetChildrenSupportedItems() const
{
    return m_vType;
}

const TString FileSystemModel::GetItem(ItemType itemType) const
{
    _UpdateInfo();
    switch (itemType)
    {
    case IModel::ItemType::Size:
        return ToTString(m_upPrivate->Size);
    case IModel::ItemType::Modified:
        break;
    case IModel::ItemType::Created:
        break;
    case IModel::ItemType::Accessed:
        break;
    case IModel::ItemType::Attributes:
        break;
    default:
        break;
    }
    return TString();
}

void FileSystemModel::_UpdateInfo() const
{
    if (m_upPrivate->IsValid)
    {
        return;
    }
    wxFileName info(m_upPrivate->FullPath);
    info.Normalize();
    m_upPrivate->FullPath = info.GetFullPath();
    //m_upPrivate->IsFolder = info.DirExists(m_upPrivate->FullPath);
    m_upPrivate->IsFolder = info.IsDir();
    if (m_upPrivate->IsFolder)
    {
        if (info.GetDirCount())
        {
            m_upPrivate->Name = info.GetDirs().Last();
        }
    }
    else
    {
        m_upPrivate->Name = info.GetFullName();
        auto size = info.GetSize().GetValue();
        if (size != wxInvalidSize)
        {
            m_upPrivate->Size = size;
        }
    }
    m_upPrivate->IsValid = true;
}

void FileSystemModel::_UpdateChildren() const
{
    if (!m_upPrivate->IsChildrenValid)
    {
        m_upPrivate->DirChildren.clear();
        m_upPrivate->FileChidren.clear();
        if (IsFolder())
        {
            wxDir dir(GetFullPath());
            wxString wxstrFileName;
            for (bool isSuccess = dir.GetFirst(&wxstrFileName)
                ; isSuccess
                ; isSuccess = dir.GetNext(&wxstrFileName))
            {
                wxString wstrFullPath = dir.GetNameWithSep() + wxstrFileName;
                if (wxFileName::DirExists(wstrFullPath))
                {
                    m_upPrivate->DirChildren.push_back(make_shared<FileSystemModel>(wstrFullPath.ToStdWstring()));
                }
                else
                {
                    m_upPrivate->FileChidren.push_back(make_shared<FileSystemModel>(wstrFullPath.ToStdWstring()));
                }
            }
        }
        else
        {
            throw "not implemented yet";
        }
        m_upPrivate->IsChildrenValid = true;
    }
}

wxIcon FileSystemModel::__GetFileIcon(TString tstrExtension, bool isFolder, bool isSmallIcon)
{
    SHFILEINFO info = {};
    wxIcon icon;
    if (SUCCEEDED(SHGetFileInfo(tstrExtension.c_str()
        , isFolder ? FILE_ATTRIBUTE_DIRECTORY : FILE_ATTRIBUTE_NORMAL
        , &info
        , sizeof(info)
        , SHGFI_USEFILEATTRIBUTES | SHGFI_ICON | (isSmallIcon ? SHGFI_SMALLICON : 0))))
    {
        icon.CreateFromHICON(info.hIcon);
    }
    return icon;
}

std::vector<IModel::ItemType> FileSystemModel::m_vType = {
    IModel::ItemType::Size,
    IModel::ItemType::Modified,
    IModel::ItemType::Created,
    IModel::ItemType::Accessed,
    IModel::ItemType::Attributes,
};
