#include "stdwx.h"
#include "ModelBase.h"

#include "SevenZipCore/CommonHelper.h"
#include "SevenZipCore/ComPtr.h"
#include "SevenZipCore/OpenCallback.h"
#include "SevenZipCore/Platform.h"

#include "ArchiveModel.h"
#include "ComputerModel.h"
#include "FileHelper.h"
#include "FileInfo.h"
#include "FolderModel.h"

using namespace std;

bool LocaleCompare(
    const TString &tstrLeft, const TString & tstrRight, bool isAscending)
{
    if (tstrLeft.empty() || tstrRight.empty())
    {
        return OrderCompare(tstrLeft, tstrRight, isAscending);
    }

#ifdef __WXMSW__
    switch (::CompareString(
        GetUserDefaultLCID(),
        0, tstrLeft.c_str(),
        tstrLeft.size(),
        tstrRight.c_str(),
        tstrRight.size())) {
    case CSTR_LESS_THAN:
        return isAscending;
    case CSTR_GREATER_THAN:
        return !isAscending;
    default:
        return false;
    }
#endif
}

const ModelBase::Models &ModelBase::GetChildren() const
{
    if (!m_isInitialized)
    {
        m_vspChildren = _InitializeChildren();
        m_isInitialized = true;
    }
    return m_vspChildren;
}

TString ModelBase::GetItem(ModelItemType itemType) const
{
    switch (itemType)
    {
    case ModelItemType::Name:
        return m_tstrFileName;
    case ModelItemType::Type:
        return m_tstrType;
    }
    return wxEmptyString;
}

void ModelBase::OpenExternal() const
{
    Helper::OpenFileExternal(GetPath());
}

bool ModelBase::Compare(const ModelBase &otherModel,
    ModelItemType itemType,
    bool isAscending) const
{
    switch (itemType)
    {
    case ModelItemType::Name:
        return LocaleCompare(
            m_tstrFileName, otherModel.m_tstrFileName, isAscending);
    //case ModelItemType::Size:
    //    return COMPARE(m_un64Size, otherModel.m_un64Size, isAscending);
    //case ModelItemType::PackedSize:
    //    return COMPARE(
    //        m_un64PackedSize, otherModel.m_un64PackedSize, isAscending);
    case ModelItemType::Type:
        return LocaleCompare(
            GetItem(itemType), otherModel.GetItem(itemType), isAscending);
    //case ModelItemType::Modified:
    //    return COMPARE(m_dtModified, otherModel.m_dtModified, isAscending);
    //case ModelItemType::Created:
    //    return COMPARE(m_dtCreated, otherModel.m_dtCreated, isAscending);
    //case ModelItemType::Accessed:
    //    return COMPARE(m_dtAccessed, otherModel.m_dtAccessed, isAscending);
    }
    return false;
}

ModelBase::ModelBase(TString tstrLocation,
    TString tstrFileName,
    TString tstrType,
    bool isDirectory,
    bool isVirtual)
    : m_tstrFileName(move(tstrFileName))
    , m_tstrLocation(move(tstrLocation))
    , m_tstrType(move(tstrType))
    , m_isDirectory(isDirectory)
    , m_isVirtual(isVirtual)
{

}

ModelBase::ModelBase(TString tstrLocation,
    TString tstrFileName,
    bool isDirectory,
    bool isVirtual)
    : m_tstrFileName(move(tstrFileName))
    , m_tstrLocation(move(tstrLocation))
    , m_tstrType(FileInfo::GetType(m_tstrFileName, isDirectory, isVirtual))
    , m_isDirectory(isDirectory)
    , m_isVirtual(isVirtual)
{

}

shared_ptr<ModelBase> GetModelFromPath(
    TString tstrPath, bool isTryOpenArchive/* = true*/)
{
    if (1 == tstrPath.size() && wxIsPathSeparator(tstrPath[0]))
    {
#ifdef __WXMSW__
        return make_shared<ComputerModel>();
#else
        return make_shared<FolderModel>(wxT("/"));
#endif
    }
    TString tstrOriginalPath = tstrPath;
    tstrPath = Helper::GetCanonicalPath(move(tstrPath));
    while (!tstrPath.empty())
    {
        int attributes = Helper::GetFileAttributes(tstrPath);
        if (attributes & EIGHT_FILE_STATUS_DIRECTORY)
        {
            return make_shared<FolderModel>(tstrPath);
        }
        else if (isTryOpenArchive && attributes & EIGHT_FILE_STATUS_FILE)
        {
            auto spModel = make_shared<ArchiveModel>(
                nullptr,
                tstrPath,
                wxEmptyString,
                tstrPath,
                SevenZipCore::MakeComPtr(new SevenZipCore::OpenCallback));
            return GetModelFromPath(spModel, tstrOriginalPath);
        }
        tstrPath = SevenZipCore::Helper::RemovePathSlash(move(tstrPath));
        auto szLocation = tstrPath.find_last_of(FOLDER_POSSIBLE_SEPARATOR);
        if (TString::npos == szLocation)
        {
            break;
        }
        tstrPath = tstrPath.substr(0, szLocation);
    }
}

shared_ptr<ModelBase> GetModelFromPath(
    shared_ptr<ModelBase> spModel, TString tstrPath)
{
    if (!spModel || !spModel->IsVirtual())
    {
        spModel = GetModelFromPath(tstrPath);
    }
    if (!spModel->IsVirtual())
    {
        return spModel;
    }
    tstrPath = Helper::GetCanonicalPath(move(tstrPath));
    auto vtstrPathPart = SevenZipCore::Helper::SplitString(
        tstrPath, FOLDER_POSSIBLE_SEPARATOR, true);
    auto vtstrModelPathPart = SevenZipCore::Helper::SplitString(
        spModel->GetPath(), FOLDER_POSSIBLE_SEPARATOR, true);
    auto szMinSize = min(vtstrPathPart.size(), vtstrModelPathPart.size());
    int i = 0;
    while (i != szMinSize && Helper::IsPathEqual(
        vtstrPathPart[i], vtstrModelPathPart[i]))
    {
        ++i;
    }
    int nDiffCount = vtstrModelPathPart.size() - i;
    while (nDiffCount--)
    {
        spModel = spModel->GetParent();
        if (!spModel->IsVirtual())
        {
            return GetModelFromPath(tstrPath);
        }
    }
    while (i != vtstrPathPart.size())
    {
        auto children = spModel->GetChildren();
        auto iter = find_if(children.cbegin(), children.cend(),
            [&vtstrPathPart, i](const shared_ptr<ModelBase> &spModel) {
            return spModel->GetFileName() == vtstrPathPart[i];
        });
        if (children.cend() == iter || !(*iter)->IsDirectory())
        {
            break;
        }
        else
        {
            spModel = *iter;
        }
        ++i;
    }
    return spModel;
}
