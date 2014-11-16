// FolderEntry.h

#ifndef FOLDERENTRY_H
#define FOLDERENTRY_H

#include "EntryBase.h"

class FolderEntry
    : public EntryBase
{
public:
    FolderEntry(TString tstrLocation,
        TString tstrName,
        bool isDirectory,
        wxULongLong_t un64Size,
        wxDateTime dtAccessed,
        wxDateTime dtModified,
        wxDateTime dtCreated);
    virtual ~FolderEntry() { }

    virtual std::shared_ptr<ModelBase> GetContainer() const;
    virtual std::shared_ptr<ModelBase> GetModel() const;
    virtual bool CanExtract() const;
    virtual TString GetItem(EntryItemType itemType) const;
    virtual bool Compare(
        const EntryBase &otherEntry,
        EntryItemType itemType,
        bool isAscending) const;

private:
    wxULongLong_t m_un64Size;
    wxDateTime m_dtModified;
    wxDateTime m_dtCreated;
    wxDateTime m_dtAccessed;

};

#endif // FOLDERENTRY_H
