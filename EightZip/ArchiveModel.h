// ArchiveModel.h

#ifndef ARCHIVEMODEL_H
#define ARCHIVEMODEL_H

#include <memory>

#include "SevenZipCore/Archive.h"

#include "IExtractable.h"
#include "ModelBase.h"
#include "TempFolder.h"

namespace SevenZipCore
{
    class ArchiveFile;
    class ArchiveFolder;
    struct IInStream;
    struct IArchiveOpenCallback;
    class IExtractIndicator;
}

class ArchiveModel;

class ArchiveEntry
    : public EntryBase
{
public:
    ArchiveEntry(
        std::weak_ptr<ArchiveModel> wpParent,
        std::shared_ptr<SevenZipCore::ArchiveFile> spArchiveFile,
        TString tstrPath,
        bool isDirectory);
    virtual ~ArchiveEntry() {}

    virtual int GetIconIndex() const;
    virtual TString GetItem(ItemType itemType) const;
    virtual std::shared_ptr<IModel> GetModel();
    virtual bool CanExtract() const { return true; }

    virtual UINT32 GetArchiveIndex() const;
    virtual void OpenExternal();

private:
    std::weak_ptr<ArchiveModel> m_wpParent;
    std::shared_ptr<SevenZipCore::ArchiveFile> m_spArchiveFile;
    std::unique_ptr<TempFolder> m_upTempFolder;

    bool m_canOpenInternal = false;

    void __ExtractToTempFolder();

};

class ArchiveModel
    : public ModelBase
    , public IExtractable
    , public std::enable_shared_from_this<ArchiveModel>
{
public:
    // For stream.
    // Virtual path should be a file path and not end with a slash.
    ArchiveModel(
        std::shared_ptr<IModel> spParent,
        TString tstrVirtualPath,
        TString tstrInternalPath,
        std::shared_ptr<SevenZipCore::IInStream> cpStream,
        std::shared_ptr<SevenZipCore::IArchiveOpenCallback> cpCallback);
    ArchiveModel(
        std::shared_ptr<IModel> spParent,
        TString tstrVirtualPath,
        TString tstrInternalPath,
        TString tstrRealPath,
        std::shared_ptr<SevenZipCore::IArchiveOpenCallback> cpCallback);
    ArchiveModel(
        std::shared_ptr<IModel> spParent,
        TString tstrPath,
        TString tstrInternalPath,
        std::shared_ptr<SevenZipCore::ArchiveFolder> spArchiveFolder);
    virtual ~ArchiveModel() {}

    virtual std::shared_ptr<IModel> GetParent() const;
    virtual const std::vector<IEntry::ItemType> &GetSupportedItems() const;
    virtual bool IsArchive() const { return true; }

    virtual void Extract(TString tstrPath,
        SevenZipCore::IExtractIndicator *pExtractIndicator) const;
    virtual TString Extract(UINT32 un32ArchiveIndex, TString tstrPath,
        SevenZipCore::IExtractIndicator *pExtractIndicator) const;
    virtual void Extract(const std::vector<UINT32> &vun32ArchiveIndex,
        TString tstrPath,
        SevenZipCore::IExtractIndicator *pExtractIndicator) const;

    virtual const TString &GetInternalPath() const;

    // Should call LoadChildren() manually since shared_from_this() is not
    // available in constructor.
    void LoadChildren();

private:
    std::shared_ptr<IModel> m_spParent;
    std::shared_ptr<SevenZipCore::ArchiveFolder> m_spArchiveFolder;
    std::shared_ptr<SevenZipCore::Archive> m_spArchive;

    TString m_tstrInternalPath;

};

#endif // ARCHIVEMODEL_H
