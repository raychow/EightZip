// ArchiveModel.h

#ifndef ARCHIVEMODEL_H
#define ARCHIVEMODEL_H

#include <memory>

#include "SevenZipCore/Archive.h"

#include "ModelBase.h"

class ArchiveEntry
    : public EntryBase
{
public:
    ArchiveEntry(std::shared_ptr<IModel> spParent
        , std::shared_ptr<SevenZipCore::ArchiveFile> spArchiveFile
        , TString tstrPath
        , bool isDirectory);

    virtual std::shared_ptr<IModel> GetModel() const;

private:
    std::shared_ptr<IModel> m_spParent;
    std::shared_ptr<SevenZipCore::ArchiveFile> m_spArchiveFile;

};

class ArchiveModel
    : public ModelBase
{
public:
    ArchiveModel(std::shared_ptr<IModel> spParent
        , TString tstrPath
        , std::shared_ptr<SevenZipCore::Codecs> cpCodecs
        , TString tstrTempFullPath
        , std::shared_ptr<SevenZipCore::IArchiveOpenCallback> cpCallback);
    ArchiveModel(std::shared_ptr<IModel> spParent
        , TString tstrPath
        , std::shared_ptr<SevenZipCore::ArchiveFolder> spArchiveFolder);
    virtual ~ArchiveModel() {}

    virtual std::shared_ptr<IModel> GetParent() const;
    virtual const std::vector<IEntry::ItemType> &GetSupportedItems() const;

private:
    std::shared_ptr<IModel> m_spParent;
    std::shared_ptr<SevenZipCore::ArchiveFolder> m_spArchiveFolder;

};

#endif // ARCHIVEMODEL_H
