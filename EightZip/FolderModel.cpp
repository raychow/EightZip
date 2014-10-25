#include "stdwx.h"
#include "FolderModel.h"

#include "SevenZipCore/ArchiveEntry.h"
#include "SevenZipCore/CommonHelper.h"

#include "ArchiveModel.h"
#include "CodecsLoader.h"
#include "ComputerModel.h"
#include "Exception.h"
#include "FileFinder.h"
#include "FileHelper.h"
#include "FileInfo.h"
#include "FileModel.h"

using namespace std;

FolderModel::FolderModel(TString tstrPath)
    : ModelBase(wxEmptyString, wxEmptyString, wxEmptyString, true, false)
{
    FileInfo fileInfo(move(tstrPath));
    if (fileInfo.IsOK())
    {
        tstrPath = SevenZipCore::Helper::RemovePathSlash(
            fileInfo.GetCanonicalPath());
#ifdef __WXMSW__
        if (tstrPath.size() == 2 && tstrPath.back() == ':')
#else
        if (tstrPath.empty())
#endif
        {
            tstrPath.push_back(wxFILE_SEP_PATH);
            SetFileName(tstrPath);
        }
        else
        {
            SetLocation(Helper::GetParentPath(tstrPath));
            SetFileName(SevenZipCore::Helper::GetFileName(tstrPath));
        }
        SetType(fileInfo.GetType());
        m_dtAccessed = fileInfo.GetAccessed();
        m_dtModified = fileInfo.GetModified();
        m_dtCreated = fileInfo.GetCreated();
    }
    else
    {
        throw ModelException("Cannot access the specified path.");
    }
}

FolderModel::FolderModel(TString tstrLocation,
    TString tstrFileName,
    wxDateTime dtAccessed,
    wxDateTime dtModified,
    wxDateTime dtCreated)
    : ModelBase(tstrLocation, tstrFileName, true, false)
    , m_dtAccessed(dtAccessed)
    , m_dtModified(dtModified)
    , m_dtCreated(dtCreated)
{

}

shared_ptr<ModelBase> FolderModel::GetParent() const
{
    auto tstrParentPath = GetLocation();
    if (tstrParentPath.empty())
    {
#ifdef __WXMSW__
        return make_shared<ComputerModel>();
#else
        return make_shared<FolderModel>(wxFILE_SEP_PATH);
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

const vector<ModelItemType> &FolderModel::GetSupportedItems() const
{
    static vector<ModelItemType> vType = {
        ModelItemType::Name,
        ModelItemType::Type,
        ModelItemType::Size,
        ModelItemType::Modified,
        ModelItemType::Created,
        ModelItemType::Accessed,
    };
    return vType;
}

bool FolderModel::CanExtract() const
{
    return false;
}

const ModelBase::Models FolderModel::GetChildren() const
{
    Models result;
    auto tstrPath = GetPath();
    FileFinder finder(GetPath());
    tstrPath = SevenZipCore::Helper::RemovePathSlash(move(tstrPath));
    while (finder.FindNext())
    {
        if (finder.IsDirectory())
        {
            result.push_back(make_shared<FolderModel>(
                tstrPath,
                finder.GetFileName(),
                finder.GetAccessed(),
                finder.GetModified(),
                finder.GetCreated()));
        }
        else
        {
            result.push_back(make_shared<FileModel>(
                tstrPath,
                finder.GetFileName(),
                finder.GetAccessed(),
                finder.GetModified(),
                finder.GetCreated()));
        }
    }
    return result;
}