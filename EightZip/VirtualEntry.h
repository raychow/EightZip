// VirtualEntry.h

#ifndef VIRTUALENTRY_H
#define VIRTUALENTRY_H

#include <boost/optional.hpp>

#include "SevenZipCore/ArchiveFile.h"

#include "EntryBase.h"
#include "TempFolder.h"

class ProgressDialog;
class VirtualModel;

class VirtualEntry
    : public EntryBase
{
public:
    VirtualEntry(TString tstrLocation, TString tstrName, bool isDirectory,
        std::weak_ptr<VirtualModel> wpParent,
        SevenZipCore::ArchiveFile &archiveFile);
    virtual ~VirtualEntry() { }

    TString GetInternalLocation() const;

    virtual std::shared_ptr<ModelBase> GetContainer() const;
    virtual std::shared_ptr<ModelBase> GetModel() const;
    virtual bool CanExtract() const { return true; }
    virtual void OpenExternal() const;
    virtual TString GetItem(EntryItemType itemType) const;
    virtual bool Compare(const EntryBase &otherEntry,
        EntryItemType itemType,
        bool isAscending) const;

    inline SevenZipCore::ArchiveFile &GetArchiveFile() const
    {
        return m_archiveFile;
    }

private:
    wxULongLong_t m_un64Size;
    wxULongLong_t m_un64PackedSize;
    wxDateTime m_dtModified;
    boost::optional<UINT32> m_oun32CRC;

    std::weak_ptr<VirtualModel> m_wpParent;
    SevenZipCore::ArchiveFile &m_archiveFile;
    mutable std::unique_ptr<TempFolder> m_upTempFolder;

    void __ExtractToTempFolder(ProgressDialog *pProgressDialog) const;

    std::shared_ptr<VirtualModel> __GetVirtualModel(
        ProgressDialog *pProgressDialog) const;

};

#endif // VIRTUALENTRY_H
