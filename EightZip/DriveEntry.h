// DriveEntry.h

#ifndef DRIVEENTRY_H
#define DRIVEENTRY_H

#include "EntryBase.h"

class DriveEntry
    : public EntryBase
{
public:
    DriveEntry(TString tstrName);
    virtual ~DriveEntry() { }

    virtual std::shared_ptr<ModelBase> GetModel() const;
    virtual bool IsOpenExternal() { return false; }
    virtual bool CanExtract() const { return false; }
    virtual TString GetItem(EntryItemType itemType) const;
    virtual bool Compare(
        const EntryBase &otherEntry,
        EntryItemType itemType,
        bool isAscending) const;

private:
    wxULongLong_t m_un64TotalSize = 0;
    wxULongLong_t m_un64FreeSpace = 0;

};

#endif // DRIVEENTRY_H
