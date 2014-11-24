#include "stdwx.h"
#include "ModelBase.h"

#include "SevenZipCore/CommonHelper.h"
#include "SevenZipCore/ComPtr.h"
#include "SevenZipCore/OpenCallback.h"
#include "SevenZipCore/Platform.h"

#include "EntryBase.h"
#include "DriveModel.h"
#include "FileHelper.h"
#include "FolderModel.h"
#include "OpenIndicator.h"
#include "VirtualRootModel.h"

using namespace std;

ModelBase::ModelBase(TString tstrLocation, TString tstrName)
    : m_tstrLocation(SevenZipCore::Helper::MakePathSlash(move(tstrLocation)))
    , m_tstrName(move(tstrName))
{

}

shared_ptr<ModelBase> GetModelFromPath(
    TString tstrPath, bool isTryOpenArchive/* = true*/)
{
    if (tstrPath.size() != 1 || TString::npos == tstrPath.find_first_of(
        FOLDER_POSSIBLE_SEPARATOR))
    {
        TString tstrOriginalPath = tstrPath;
        tstrPath = Helper::GetCanonicalPath(move(tstrPath));
        while (!tstrPath.empty())
        {
            int attributes = Helper::GetFileAttributes(tstrPath);
            if (attributes & EIGHT_FILE_STATUS_DIRECTORY)
            {
                return make_unique<FolderModel>(tstrPath);
            }
            else if (isTryOpenArchive && attributes & EIGHT_FILE_STATUS_FILE)
            {
                auto openIndicator = OpenIndicator { nullptr };
                auto upCallback = SevenZipCore::MakeUniqueCom(
                    new SevenZipCore::OpenCallback { &openIndicator });
                auto spModel = make_shared<VirtualRootModel>(
                    Helper::GetLocation(tstrPath),
                    SevenZipCore::Helper::GetFileName(tstrPath),
                    tstrPath,
                    nullptr,
                    upCallback.get());
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
#ifdef __WXMSW__
    return make_unique<DriveModel>();
#else
    return make_unique<FolderModel>(wxT("/"));
#endif
}

shared_ptr<ModelBase> GetModelFromPath(
    shared_ptr<ModelBase> spModel, TString tstrPath)
{
    if (!spModel || !spModel->IsArchive())
    {
        spModel = GetModelFromPath(tstrPath);
    }
    if (!spModel->IsArchive())
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
        if (!spModel->IsArchive())
        {
            return GetModelFromPath(tstrPath);
        }
    }
    while (i != vtstrPathPart.size())
    {
        auto iter = find_if(spModel->cbegin(), spModel->cend(),
            [&vtstrPathPart, i](const ModelBase::EntryVector::value_type &upEntry) {
            return upEntry->GetName() == vtstrPathPart[i];
        });
        if (spModel->cend() == iter || !(*iter)->IsDirectory())
        {
            break;
        }
        else
        {
            spModel = (*iter)->GetModel();
        }
        ++i;
    }
    return spModel;
}
