// VirtualModel.h

#ifndef VIRTUALMODEL_H
#define VIRTUALMODEL_H

#include <memory>

#include "SevenZipCore/Archive.h"

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

class ArchiveProperty;

class VirtualModel
    : public ModelBase
    , public std::enable_shared_from_this<VirtualModel>
{
public:
    // Open archive folder.
    VirtualModel(TString tstrLocation,
        TString tstrInternalLocation,
        TString tstrName,
        std::shared_ptr<VirtualModel> spParent,
        SevenZipCore::ArchiveFolder &archiveFolder);
    virtual ~VirtualModel() {}

    virtual std::shared_ptr<ModelBase> GetParent() const;
    virtual const std::vector<EntryItemType> &GetSupportedItems() const;
    virtual bool IsArchive() const { return true; }

    virtual const TString &GetInternalLocation() const
    {
        return m_tstrInternalLocation;
    }

    inline SevenZipCore::Archive &GetArchive() const
    {
        return m_archive;
    }
    inline ArchiveProperty &GetProperty() const
    {
        return m_property;
    }
    inline SevenZipCore::ArchiveFolder &GetArchiveFolder() const
    {
        return m_archiveFolder;
    }

    inline bool IsRoot() const { return !m_spParent; }

protected:
    // Open archive root.
    VirtualModel(TString tstrLocation,
        TString tstrName,
        std::shared_ptr<VirtualModel> spParent,
        SevenZipCore::Archive &archive,
        ArchiveProperty &property);

    virtual EntryVector _InitializeEntries() const;

private:
    SevenZipCore::Archive &m_archive;
    ArchiveProperty &m_property;
    std::shared_ptr<ModelBase> m_spParent;
    SevenZipCore::ArchiveFolder &m_archiveFolder;

    TString m_tstrInternalLocation;

};

#endif // VIRTUALMODEL_H
