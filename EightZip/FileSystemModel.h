// FileSystemModel.h

#ifndef FILESYSTEMMODEL_H
#define FILESYSTEMMODEL_H

#include <memory>

#include "ModelBase.h"

class FileSystemModel
    : public ModelBase
{
public:
    FileSystemModel(TString tstrParentFolder, TString tstrFileName);

    virtual const std::vector<ItemType> &GetChildrenSupportedItems() const;
    virtual TString GetItem(ItemType itemType) const;

protected:
    virtual void _UpdateInfo() const;
    virtual void _UpdateChildren() const;
private:
    static std::vector<ItemType> m_vType;

};

#endif // FILESYSTEMMODEL_H
