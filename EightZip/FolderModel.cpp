#include "stdwx.h"
#include "FolderModel.h"

#include "SevenZipCore/CommonHelper.h"

#include "DriveModel.h"
#include "Exception.h"
#include "FileFinder.h"
#include "FileHelper.h"
#include "FileInfo.h"
#include "FolderEntry.h"

using namespace std;

FolderModel::FolderModel(TString tstrPath)
    : ModelBase(wxEmptyString, wxEmptyString)
{
    FileInfo fileInfo(move(tstrPath));
    if (fileInfo.IsOK())
    {
        auto tstrCanonicalPath = SevenZipCore::Helper::RemovePathSlash(
            fileInfo.GetCanonicalPath());
#ifdef __WXMSW__
        if (2 == tstrCanonicalPath.size() && tstrCanonicalPath.back() == ':')
#else
        if (tstrCanonicalPath.empty())
#endif
        {
            tstrCanonicalPath.push_back(wxFILE_SEP_PATH);
            SetLocation(move(tstrCanonicalPath));
            SetName(wxEmptyString);
        }
        else
        {
            SetLocation(Helper::GetLocation(tstrCanonicalPath));
            SetName(SevenZipCore::Helper::GetFileName(move(tstrCanonicalPath)));
        }
    }
    else
    {
        throw ModelException("Cannot access the specified path.");
    }
}

FolderModel::FolderModel(TString tstrLocation, TString tstrName)
    : FolderModel(SevenZipCore::Helper::MakePathSlash(
    move(tstrLocation)) + tstrName)
{

}

std::shared_ptr<ModelBase> FolderModel::GetParent() const
{
    if (GetName().empty())
    {
#ifdef __WXMSW__
        return make_shared<DriveModel>();
#else
        return make_shared<FolderModel>(wxEmptyString, wxFILE_SEP_PATH);
#endif
    }
    else
    {
        auto tstrLocation = GetLocation();
        try
        {
            return make_shared<FolderModel>(tstrLocation);
        }
        catch (const ModelException &)
        {
            return GetModelFromPath(tstrLocation);
        }
    }
}

const vector<EntryItemType> &FolderModel::GetSupportedItems() const
{
    static vector<EntryItemType> vType = {
        EntryItemType::Name,
        EntryItemType::Type,
        EntryItemType::Size,
        EntryItemType::Modified,
        EntryItemType::Created,
        EntryItemType::Accessed,
    };
    return vType;
}

FolderModel::EntryVector FolderModel::_InitializeEntries() const
{
    EntryVector result;
    auto tstrPath = GetPath();
    FileFinder finder(tstrPath);
    while (finder.FindNext())
    {
        result.push_back(make_shared<FolderEntry>(
            tstrPath,
            finder.GetFileName(),
            finder.IsDirectory(),
            finder.GetSize(),
            finder.GetAccessed(),
            finder.GetModified(),
            finder.GetCreated()));
    }
    return result;
}
