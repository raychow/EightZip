#include "stdwx.h"
#include "FileModel.h"

#include "SevenZipCore/Archive.h"
#include "SevenZipCore/ArchiveFile.h"
#include "SevenZipCore/ComPtr.h"
#include "SevenZipCore/OpenCallback.h"

#include "ArchiveModel.h"
#include "CodecsLoader.h"

using namespace std;

FileModel::FileModel(TString tstrLocation,
    TString tstrFileName,
    wxDateTime dtAccessed,
    wxDateTime dtModified,
    wxDateTime dtCreated)
    : FolderModel(tstrLocation, tstrFileName, dtAccessed, dtModified, dtCreated)
{

}

const vector<ModelItemType> &FileModel::GetSupportedItems() const
{
    static vector<ModelItemType> vType = {
        ModelItemType::Name,
        ModelItemType::Type,
        ModelItemType::Size,
        ModelItemType::PackedSize,
        //ModelItemType::Modified,
        ModelItemType::CRC
    };
    return vType;
}

bool FileModel::CanExtract() const
{
    try
    {
        SevenZipCore::ArchiveEntry(
            weak_ptr<SevenZipCore::Archive>(),
            CodecsLoader::GetInstance().GetCodecs(),
            GetPath(),
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

const ModelBase::Models FileModel::GetChildren() const
{
    Models result;
    m_spArchive.reset(new SevenZipCore::Archive(
        CodecsLoader::GetInstance().GetCodecs()));
    m_spArchive->Open(GetPath(), nullptr/* TODO */);
    auto spRootFolder = m_spArchive->GetRootFolder();
    auto tstrPath = GetPath();
    for (const auto &folder : spRootFolder->GetFolders())
    {
        result.push_back(make_shared<ArchiveModel>(
            shared_from_this(),
            tstrPath,
            folder->GetName(),
            wxEmptyString,
            folder));
    }
    for (const auto &file : spRootFolder->GetFiles())
    {
        result.push_back(make_shared<ArchiveModel>(
            shared_from_this(),
            tstrPath,
            file->GetName(),
            wxEmptyString,
            file));
    }
    return result;
}
