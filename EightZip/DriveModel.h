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
    virtual std::shared_ptr<IModel> GetParent() const;
    virtual const std::vector<ItemType> &GetChildrenSupportedItems() const;

protected:
    virtual void _UpdateChildren();

private:
    static std::vector<ItemType> m_vType;

#ifdef __WXMSW__
    static std::vector<TString> __GetDrives();
#endif

};

#endif

#endif // DRIVEMODEL_H
