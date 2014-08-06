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
        TString tstrRealtivePath,
        bool isDirectory);
    virtual ~ArchiveEntry() {}

    virtual int GetIconIndex() const;
    virtual TString GetItem(ItemType itemType) const;
    virtual std::shared_ptr<IModel> GetModel();

    void Open();

    virtual void OpenExternal() const;

    virtual TString GetInternalPath() const { return m_tstrInternalPath; }

private:
    std::weak_ptr<IModel> m_wpParent;
    std::shared_ptr<SevenZipCore::ArchiveFile> m_spArchiveFile;
    TString m_tstrInternalPath;
    TempFolder m_tempFolder;

    bool m_canOpenInternal = false;
};

class ArchiveModel
    : public ModelBase
    , public std::enable_shared_from_this<ArchiveModel>
{
public:
    ArchiveModel(
        std::shared_ptr<IModel> spParent,
        TString tstrPath,
        TString tstrInternalPath,
        TString tstrTempFullPath,
        std::shared_ptr<SevenZipCore::IArchiveOpenCallback> cpCallback);
    ArchiveModel(
        std::shared_ptr<IModel> spParent,
        TString tstrPath,
        TString tstrInternalPath,
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

    TString m_tstrInternalPath;
};

#endif // ARCHIVEMODEL_H
