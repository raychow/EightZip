// VirtualEntry.h

#ifndef VIRTUALENTRY_H
#define VIRTUALENTRY_H

#include <boost/optional.hpp>

#include "SevenZipCore/ArchiveFile.h"

#include "EntryBase.h"
#include "TempFolder.h"

class VirtualModel;

class VirtualEntry
    : public EntryBase
{
public:
    VirtualEntry(TString tstrLocation, TString tstrName, bool isDirectory,
        std::weak_ptr<VirtualModel> wpParent,
        std::shared_ptr<SevenZipCore::ArchiveFile> spArchiveFile);
    virtual ~VirtualEntry() { }

    TString GetInternalLocation() const;

    virtual std::shared_ptr<ModelBase> GetModel() const;
    virtual bool CanExtract() const { return true; }
    virtual TString GetItem(EntryItemType itemType) const;
    virtual bool Compare(const EntryBase &otherEntry,
        EntryItemType itemType,
        bool isAscending) const;

private:
    wxULongLong_t m_un64Size;
    wxULongLong_t m_un64PackedSize;
    wxDateTime m_dtModified;
    boost::optional<UINT32> m_oun32CRC;

    std::weak_ptr<VirtualModel> m_wpParent;
    std::shared_ptr<SevenZipCore::ArchiveFile> m_spArchiveFile;
    mutable std::unique_ptr<TempFolder> m_upTempFolder;

    void __ExtractToTempFolder() const;

};

#endif // VIRTUALENTRY_H
