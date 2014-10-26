// DriveModel.h

#ifndef DRIVEMODEL_H
#define DRIVEMODEL_H

#ifdef __WXMSW__

#include <memory>

#include "ModelBase.h"

class DriveModel
    : public ModelBase
{
public:
    DriveModel();

    virtual std::shared_ptr<ModelBase> GetParent() const { return nullptr; }
    virtual const std::vector<EntryItemType> &GetSupportedItems() const;
    virtual bool IsArchive() const { return false; }

protected:
    virtual EntryVector _InitializeEntries() const;

};

#endif

#endif // DRIVEMODEL_H
