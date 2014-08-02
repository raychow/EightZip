#include "stdwx.h"
#include "FolderModel.h"

#include "ArchiveModel.h"
#include "DriveModel.h"
#include "Exception.h"
#include "FileFinder.h"
#include "FileInfo.h"

using namespace std;

FolderEntry::FolderEntry(
    TString tstrPath,
    TString tstrName,
    bool isDirectory,
    wxULongLong_t un64Size,
    wxDateTime dtAccessed,
    wxDateTime dtModified,
    wxDateTime dtCreated)
{
    m_tstrPath = move(tstrPath);
    m_tstrName = move(tstrName);
    m_isDirectory = isDirectory;
    m_un64Size = un64Size;
    m_dtAccessed = dtAccessed;
    m_dtModified = dtModified;
    m_dtCreated = dtCreated;
    m_tstrType = FileInfo::GetType(GetFullPath(), IsDirectory());
}

bool FolderEntry::IsOpenDirectly() const
{
    return false;
}

std::shared_ptr<IModel> FolderEntry::GetModel() const
{
    auto tstrFullPath = GetFullPath();
    if (IsDirectory())
    {
        return make_shared<FolderModel>(tstrFullPath);
    }
    else
    {
        TString tstrExecutablePath
            = wxStandardPaths::Get().GetExecutablePath();
        tstrExecutablePath = tstrExecutablePath.substr(
            0, tstrExecutablePath.find_last_of(wxFILE_SEP_PATH) + 1);
        auto result = make_shared<ArchiveModel>(
            nullptr,
            tstrFullPath + wxFILE_SEP_PATH,
            TString(),
            SevenZipCore::MakeComPtr(
            new SevenZipCore::Codecs(tstrExecutablePath)),
            tstrFullPath,
            nullptr);
        result->LoadChildren();
        return result;
    }
}

FolderModel::FolderModel(TString tstrPath)
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
            m_vspEntry.push_back(make_shared<FolderEntry>(
                tstrPath + wxFILE_SEP_PATH,
                finder.GetFileName(),
                finder.IsDirectory(),
                finder.GetSize(),
                finder.GetAccessed(),
                finder.GetModified(),
                finder.GetCreated()));
        }
    }
    else
    {
        throw ModelException("Cannot access the specified path.");
    }
}

std::shared_ptr<IModel> FolderModel::GetParent() const
{
    auto tstrParentPath = m_tstrPath.substr(
        0, m_tstrPath.find_last_of(wxFILE_SEP_PATH) + 1);
    if (tstrParentPath.empty())
    {
        return make_shared<DriveModel>();
    }
    else
    {
        return make_shared<FolderModel>(tstrParentPath);
    }
}

const vector<IEntry::ItemType> &FolderModel::GetSupportedItems() const
{
    static vector<IEntry::ItemType> vType = {
        IEntry::ItemType::Name,
        IEntry::ItemType::Type,
        IEntry::ItemType::Size,
        IEntry::ItemType::Modified,
        IEntry::ItemType::Created,
        IEntry::ItemType::Accessed,
    };
    return vType;
}