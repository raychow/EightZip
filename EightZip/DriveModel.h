// DriveModel.h

#ifndef DRIVEMODEL_H
#define DRIVEMODEL_H

#ifdef __WXMSW__

#include <memory>

#include "ModelBase.h"

class DriveEntry
    : public EntryBase
{
public:
    DriveEntry(TString tstrName,
        wxULongLong_t un64TotalSize,
        wxULongLong_t un64FreeSpace,
        TString tstrType);

    virtual std::shared_ptr<IModel> GetModel();
    virtual bool IsOpenExternal() { return false; }
    virtual bool CanExtract() const { return false; }

};

class DriveModel
    : public ModelBase
{
public:
    DriveModel();

    virtual std::shared_ptr<IModel> GetParent() const;
    virtual const EntryVector &GetEntries() const { return m_vspEntry; }
    virtual const std::vector<IEntry::ItemType> &GetSupportedItems() const;
    virtual bool IsArchive() const { return false; }

private:
    static std::vector<TString> __GetDrives();

    EntryVector m_vspEntry;

};

#endif

#endif // DRIVEMODEL_H
