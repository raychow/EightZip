// ComputerModel.h

#ifndef COMPUTERMODEL_H
#define COMPUTERMODEL_H

#ifdef __WXMSW__

#include <memory>

#include "ModelBase.h"

class ComputerModel
    : public ModelBase
{
public:
    ComputerModel();

    virtual std::shared_ptr<ModelBase> GetParent() const;
    virtual const std::vector<ModelItemType> &GetSupportedItems() const;
    virtual bool CanExtract() const;
    virtual const Models GetChildren() const;

    virtual ~ComputerModel() { }

};

#endif

#endif // COMPUTERMODEL_H
