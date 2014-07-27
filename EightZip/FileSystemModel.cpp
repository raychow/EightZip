#include "stdwx.h"
#include "FileSystemModel.h"

#include "DriveModel.h"
#include "Exception.h"
#include "FileFinder.h"
#include "FileInfo.h"

using namespace std;

FileSystemEntry::FileSystemEntry(TString tstrPath, TString tstrName, bool isDirectory, wxULongLong_t un64Size, wxDateTime dtAccessed, wxDateTime dtModified, wxDateTime dtCreated, TString tstrType)
{
    m_tstrPath = tstrPath;
    m_tstrName = tstrName;
    m_isDirectory = isDirectory;
    m_un64Size = un64Size;
    m_dtAccessed = dtAccessed;
    m_dtModified = dtModified;
    m_dtCreated = dtCreated;
    m_tstrType = tstrType;
}

std::shared_ptr<IModel> FileSystemEntry::GetModel() const
{
    return make_shared<FileSystemModel>(GetFullPath());
}

FileSystemModel::FileSystemModel(TString tstrPath)
{
    FileInfo fileInfo(tstrPath);
    if (fileInfo.IsOK())
    {
        tstrPath = fileInfo.GetNormalizedPath();
        while (!tstrPath.empty() && wxIsPathSeparator(tstrPath.back()))
        {
            tstrPath.pop_back();
        }
        m_tstrPath = tstrPath;
        FileFinder finder(tstrPath);
        while (finder.FindNext())
        {
            m_vspEntry.push_back(make_shared<FileSystemEntry>(tstrPath + wxFILE_SEP_PATH
                , finder.GetFileName()
                , finder.IsDirectory()
                , finder.GetSize()
                , finder.GetAccessed()
                , finder.GetModified()
                , finder.GetCreated()
                , FileInfo::GetType(tstrPath + wxFILE_SEP_PATH + finder.GetFileName(), finder.IsDirectory())));
        }
    }
    else
    {
        throw ModelException("Cannot access the specified path.");
    }
}

std::shared_ptr<IModel> FileSystemModel::GetParent() const
{
    auto tstrParentPath = m_tstrPath.substr(0, m_tstrPath.find_last_of(wxFILE_SEP_PATH) + 1);
    if (tstrParentPath.empty())
    {
        return make_shared<DriveModel>();
    }
    else
    {
        return make_shared<FileSystemModel>(tstrParentPath);
    }
}

const vector<IEntry::ItemType> &FileSystemModel::GetSupportedItems() const
{
    return m_vType;
}

std::vector<IEntry::ItemType> FileSystemModel::m_vType = {
    IEntry::ItemType::Name,
    IEntry::ItemType::Size,
    IEntry::ItemType::Type,
    IEntry::ItemType::Modified,
    IEntry::ItemType::Created,
    IEntry::ItemType::Accessed,
};