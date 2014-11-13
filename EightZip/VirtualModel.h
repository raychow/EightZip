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
    VirtualModel(TString tstrLocation,
        TString tstrName,
        std::shared_ptr<ModelBase> spParent,
        std::shared_ptr<SevenZipCore::IInStream> cpStream,
        SevenZipCore::IArchiveOpenCallback &callback);
    // Open real file.
    VirtualModel(TString tstrLocation,
        TString tstrName,
        TString tstrRealPath,
        std::shared_ptr<ModelBase> spParent,
        SevenZipCore::IArchiveOpenCallback &callback);
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

    inline std::shared_ptr<SevenZipCore::Archive> GetArchive() const
    {
        return m_spArchive;
    }
    inline SevenZipCore::ArchiveFolder &GetArchiveFolder() const
    {
        return m_archiveFolder;
    }

    inline bool IsRoot() const { return !m_spParent; }

protected:
    virtual EntryVector _InitializeEntries() const;

private:
    std::shared_ptr<SevenZipCore::Archive> m_spArchive;
    std::shared_ptr<ModelBase> m_spParent;
    SevenZipCore::ArchiveFolder &m_archiveFolder;

    TString m_tstrInternalLocation;

    std::shared_ptr<SevenZipCore::ArchiveExtractCallback>  __CreateCallback(
        TString tstrPath,
        SevenZipCore::IExtractIndicator *pExtractIndicator) const;

};

#endif // VIRTUALMODEL_H
