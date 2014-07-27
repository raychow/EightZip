#include "stdwx.h"
#include "FileSystemModel.h"

#include "DriveModel.h"
#include "FileFinder.h"
#include "FileInfo.h"

using namespace std;

FileSystemModel::FileSystemModel(TString tstrPath)
{
    FileInfo fileInfo(tstrPath);
    if (fileInfo.IsOK())
    {
        tstrPath = fileInfo.GetNormalizedPath();
        while (wxIsPathSeparator(tstrPath.back()))
        {
            tstrPath.pop_back();
        }
        auto szSeparatorLocation = tstrPath.find_last_of(wxFILE_SEP_PATH);
        m_tstrPath = tstrPath.substr(0, szSeparatorLocation + 1);
        m_tstrName = tstrPath.substr(szSeparatorLocation + 1);
        if (m_tstrName.empty())
        {
            m_tstrName = move(m_tstrPath);
            while (wxIsPathSeparator(m_tstrName.back()))
            {
                m_tstrName.pop_back();
            }
        }
        m_isDirectory = fileInfo.IsDirectory();
        m_un64Size = fileInfo.GetSize();
        m_dtAccessed = fileInfo.GetAccessed();
        m_dtModified = fileInfo.GetModified();
        m_dtCreated = fileInfo.GetCreated();
        //auto fileTypeInfo = FileTypeCache::GetInfo(IsDirectory(), GetFullPath());
        //m_nIconIndex = fileTypeInfo.GetIconIndex();
        m_tstrType = fileInfo.GetType();
    }
}

std::shared_ptr<IModel> FileSystemModel::GetParent() const
{
    if (GetPath().empty())
    {
        return make_shared<DriveModel>();
    }
    else
    {
        return make_shared<FileSystemModel>(GetPath());
    }
}

const vector<IModel::ItemType> &FileSystemModel::GetChildrenSupportedItems() const
{
    return m_vType;
}

void FileSystemModel::_UpdateChildren()
{
    if (!m_isChildrenValid)
    {
        m_vspChild.clear();
        if (IsDirectory())
        {
            wxFileName dir(GetPath());
            TString tstrParentFolder = GetFullPath() + wxFILE_SEP_PATH;
            FileFinder finder(tstrParentFolder);
            while (finder.FindNext())
            {
                auto spModel = __MakeShared();
                spModel->m_tstrPath = tstrParentFolder;
                spModel->m_tstrName = finder.GetFileName();
                spModel->m_isDirectory = finder.IsDirectory();
                spModel->m_un64Size = finder.GetSize();
                spModel->m_dtAccessed = finder.GetAccessed();
                spModel->m_dtModified = finder.GetModified();
                spModel->m_dtCreated = finder.GetCreated();
                //auto fileTypeInfo = FileTypeCache::GetInfo(spModel->IsDirectory(), spModel->GetFullPath());
                //spModel->m_nIconIndex = fileTypeInfo.GetIconIndex();
                spModel->m_tstrType = FileInfo::GetType(spModel->GetFullPath(), spModel->IsDirectory());
                m_vspChild.push_back(spModel);
            }
        }
        else
        {
            throw "not implemented yet";
        }
        m_isChildrenValid = true;
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
