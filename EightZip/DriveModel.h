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
    DriveEntry(TString tstrName
        , wxULongLong_t un64TotalSize
        , wxULongLong_t un64FreeSpace
        , TString tstrType);

    virtual std::shared_ptr<IModel> GetModel() const;
    virtual bool IsOpenExternal() { return false; }

};

class DriveModel
    : public ModelBase
{
public:
    DriveModel();

    virtual std::shared_ptr<IModel> GetParent() const;
    virtual const std::vector<IEntry::ItemType> &GetSupportedItems() const;

private:
    static std::vector<TString> __GetDrives();

};

#endif

#endif // DRIVEMODEL_H
