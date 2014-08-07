// ArchiveModel.h

#ifndef ARCHIVEMODEL_H
#define ARCHIVEMODEL_H

#include <memory>

#include "SevenZipCore/Archive.h"

#include "ModelBase.h"
#include "TempFolder.h"

class ArchiveEntry
    : public EntryBase
{
public:
    ArchiveEntry(
        std::weak_ptr<IModel> wpParent,
        std::shared_ptr<SevenZipCore::ArchiveFile> spArchiveFile,
        TString tstrPath,
        bool isDirectory);
    virtual ~ArchiveEntry() {}

    virtual int GetIconIndex() const;
    virtual TString GetItem(ItemType itemType) const;
    virtual std::shared_ptr<IModel> GetModel();

    void Open();

    virtual void OpenExternal() const;

private:
    std::weak_ptr<IModel> m_wpParent;
    std::shared_ptr<SevenZipCore::ArchiveFile> m_spArchiveFile;
    TempFolder m_tempFolder;

    bool m_canOpenInternal = false;
};

class ArchiveModel
    : public ModelBase
    , public std::enable_shared_from_this<ArchiveModel>
{
public:
    // For stream.
    // Virtual path should be a file path and not end with a slash.
    ArchiveModel(
        std::shared_ptr<IModel> spParent,
        TString tstrVirtualPath,
        std::shared_ptr<SevenZipCore::IInStream> cpStream,
        std::shared_ptr<SevenZipCore::IArchiveOpenCallback> cpCallback);
    // For temp file.
    ArchiveModel(
        std::shared_ptr<IModel> spParent,
        TString tstrVirtualPath,
        TString tstrRealPath,
        std::shared_ptr<SevenZipCore::IArchiveOpenCallback> cpCallback);
    ArchiveModel(
        std::shared_ptr<IModel> spParent,
        TString tstrPath,
        std::shared_ptr<SevenZipCore::ArchiveFolder> spArchiveFolder);
    virtual ~ArchiveModel() {}

    virtual std::shared_ptr<IModel> GetParent() const;
    virtual const std::vector<IEntry::ItemType> &GetSupportedItems() const;

    // Should call LoadChildren() manually since shared_from_this() is not
    // available in constructor.
    void LoadChildren();

private:
    std::shared_ptr<IModel> m_spParent;
    std::shared_ptr<SevenZipCore::ArchiveFolder> m_spArchiveFolder;
    std::unique_ptr<SevenZipCore::Archive> m_upArchive;

};

#endif // ARCHIVEMODEL_H
