#include "stdwx.h"
#include "FolderModel.h"

#include "SevenZipCore/ArchiveEntry.h"
#include "SevenZipCore/CommonHelper.h"
#include "SevenZipCore/ComPtr.h"
#include "SevenZipCore/OpenCallback.h"

#include "ArchiveModel.h"
#include "CodecsLoader.h"
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
    m_tstrFolder = SevenZipCore::Helper::MakePathSlash(move(tstrPath));
    m_tstrName = move(tstrName);
    m_isDirectory = isDirectory;
    m_un64Size = un64Size;
    m_dtAccessed = dtAccessed;
    m_dtModified = dtModified;
    m_dtCreated = dtCreated;
    m_tstrType = FileInfo::GetType(GetPath(), IsDirectory());
}

bool FolderEntry::IsOpenDirectly() const
{
    return false;
}

std::shared_ptr<IModel> FolderEntry::GetModel()
{
    auto tstrPath = GetPath();
    if (IsDirectory())
    {
        return make_shared<FolderModel>(tstrPath);
    }
    else
    {
        auto result = make_shared<ArchiveModel>(
            nullptr,
            tstrPath,
            wxEmptyString,
            tstrPath,
            SevenZipCore::MakeComPtr(new SevenZipCore::OpenCallback));
        result->LoadChildren();
        return result;
    }
}

bool FolderEntry::CanExtract() const
{
    if (IsDirectory())
    {
        return false;
    }
    else
    {
        auto tstrPath = GetPath();
        try
        {
            SevenZipCore::ArchiveEntry(
                weak_ptr<SevenZipCore::Archive>(),
                CodecsLoader::GetInstance().GetCodecs(),
                tstrPath,
                nullptr,
                -1,
                SevenZipCore::MakeComPtr(new SevenZipCore::OpenCallback));
            return true;
        }
        catch (const SevenZipCore::SevenZipCoreException &)
        {
            return false;
        }
    }
}

FolderModel::FolderModel(TString tstrPath)
{
    FileInfo fileInfo(move(tstrPath));
    if (fileInfo.IsOK())
    {
        m_tstrPath = SevenZipCore::Helper::RemovePathSlash(
            fileInfo.GetCanonicalPath());
#ifdef __WXMSW__
        if (m_tstrPath.size() == 2 && m_tstrPath.back() == ':')
#else
        if (m_tstrPath.empty())
#endif
        {
            m_tstrPath.push_back(wxFILE_SEP_PATH);
        }
        FileFinder finder(m_tstrPath);
        while (finder.FindNext())
        {
            m_vspEntry.push_back(make_shared<FolderEntry>(
                m_tstrPath,
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

TString FolderModel::GetParentPath() const
{
    return m_tstrPath.substr(
        0, SevenZipCore::Helper::RemovePathSlash(m_tstrPath).find_last_of(
        wxFILE_SEP_PATH) + 1);
}

std::shared_ptr<IModel> FolderModel::GetParent() const
{
    auto tstrParentPath = GetParentPath();
    if (tstrParentPath.empty())
    {
#ifdef __WXMSW__
        return make_shared<DriveModel>();
#else
        return make_shared<FolderModel>("/");
#endif
    }
    else
    {
        try
        {
            return make_shared<FolderModel>(tstrParentPath);
        }
        catch (const ModelException &)
        {
            return GetModelFromPath(tstrParentPath);
        }
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
