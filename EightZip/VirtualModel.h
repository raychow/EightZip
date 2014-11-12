// VirtualModel.h

#ifndef VIRTUALMODEL_H
#define VIRTUALMODEL_H

#include <memory>

#include "SevenZipCore/Archive.h"

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

class VirtualModel
    : public ModelBase
    , public std::enable_shared_from_this<VirtualModel>
{
public:
    // Open stream.
    VirtualModel(
        TString tstrLocation,
        TString tstrName,
        std::shared_ptr<ModelBase> spParent,
        std::shared_ptr<SevenZipCore::IInStream> cpStream,
        std::shared_ptr<SevenZipCore::IArchiveOpenCallback> cpCallback);
    // Open real file.
    VirtualModel(
        TString tstrLocation,
        TString tstrName,
        TString tstrRealPath,
        std::shared_ptr<ModelBase> spParent,
        std::shared_ptr<SevenZipCore::IArchiveOpenCallback> cpCallback);
    // Open archive folder.
    VirtualModel(
        TString tstrLocation,
        TString tstrInternalLocation,
        TString tstrName,
        std::shared_ptr<ModelBase> spParent,
        std::shared_ptr<SevenZipCore::ArchiveFolder> spArchiveFolder);
    virtual ~VirtualModel() {}

    virtual std::shared_ptr<ModelBase> GetParent() const;
    virtual const std::vector<EntryItemType> &GetSupportedItems() const;
    virtual bool IsArchive() const { return true; }

    virtual const TString &GetInternalLocation() const
    {
        return m_tstrInternalLocation;
    }

    inline std::shared_ptr<SevenZipCore::ArchiveFolder> GetArchiveFolder() const
    {
        if (!m_spArchiveFolder)
        {
            m_spArchiveFolder = m_spArchive->GetRootFolder();
        }
        return m_spArchiveFolder;
    }
    inline std::shared_ptr<SevenZipCore::Archive> GetArchive() const
    {
        return m_spArchive;
    }

    inline bool IsRoot() const { return !m_spParent; }

protected:
    virtual EntryVector _InitializeEntries() const;

private:
    std::shared_ptr<ModelBase> m_spParent;
    mutable std::shared_ptr<SevenZipCore::ArchiveFolder> m_spArchiveFolder;
    std::shared_ptr<SevenZipCore::Archive> m_spArchive;

    TString m_tstrInternalLocation;

    std::shared_ptr<SevenZipCore::ArchiveExtractCallback>  __CreateCallback(
        TString tstrPath,
        SevenZipCore::IExtractIndicator *pExtractIndicator) const;

};

#endif // VIRTUALMODEL_H
