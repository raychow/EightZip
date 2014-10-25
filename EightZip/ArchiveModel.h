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
    class ArchiveExtractCallback;
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
    virtual std::shared_ptr<IModel> GetModel() const;
    virtual bool CanExtract() const { return true; }

    UINT32 GetArchiveIndex() const;
    std::vector<UINT32> GetAllArchiveIndexes() const;
    void OpenExternal() const;

private:
    std::weak_ptr<ArchiveModel> m_wpParent;
    std::shared_ptr<SevenZipCore::ArchiveFile> m_spArchiveFile;
    mutable std::unique_ptr<TempFolder> m_upTempFolder;

    void __ExtractToTempFolder() const;

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
        shared_ptr<ModelBase> spParent,
        TString tstrVirtualLocation,
        TString tstrVirtualFileName,
        TString tstrInternalPath,
        std::shared_ptr<SevenZipCore::IInStream> cpStream);
    ArchiveModel(
        std::shared_ptr<ModelBase> spParent,
        TString tstrVirtualLocation,
        TString tstrVirtualFileName,
        TString tstrInternalPath,
        TString tstrArchivePath);
    ArchiveModel(
        std::shared_ptr<ModelBase> spParent,
        TString tstrVirtualLocation,
        TString tstrVirtualFileName,
        TString tstrInternalPath,
        std::shared_ptr<SevenZipCore::ArchiveFolder> spArchiveFolder);
    virtual ~ArchiveModel() {}

    virtual std::shared_ptr<ModelBase> GetParent() const;
    virtual const std::vector<ModelItemType> &GetSupportedItems() const;
    virtual bool CanExtract() const;

    virtual void Extract(TString tstrPath,
        SevenZipCore::IExtractIndicator *pExtractIndicator) const;
    virtual TString Extract(UINT32 un32ArchiveIndex, TString tstrPath,
        SevenZipCore::IExtractIndicator *pExtractIndicator) const;
    virtual void Extract(std::vector<UINT32> vun32ArchiveIndex,
        TString tstrPath,
        SevenZipCore::IExtractIndicator *pExtractIndicator) const;

    std::shared_ptr<SevenZipCore::Archive> GetArchive() const;
    std::vector<UINT32> GetArchiveIndexes() const;

    inline const TString &GetInternalPath() const
    {
        return m_tstrInternalPath;
    }

    virtual ~ArchiveModel() { }

private:
    wxULongLong_t m_un64Size = 0;
    wxULongLong_t m_un64PackedSize = 0;
    wxDateTime m_dtModified;
    boost::optional<UINT32> m_oun32CRC;

    std::shared_ptr<ModelBase> m_spParent;
    std::shared_ptr<SevenZipCore::ArchiveFile> m_spArchiveFile;
    std::shared_ptr<SevenZipCore::Archive> m_spArchive;
    mutable std::unique_ptr<TempFolder> m_upTempFolder;

    TString m_tstrInternalPath;

    std::shared_ptr<SevenZipCore::ArchiveExtractCallback>  __CreateCallback(
        TString tstrPath,
        SevenZipCore::IExtractIndicator *pExtractIndicator) const;

    void __ExtractToTempFolder() const;


};

#endif // ARCHIVEMODEL_H
