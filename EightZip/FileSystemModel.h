// FileSystemModel.h

#ifndef FILESYSTEMMODEL_H
#define FILESYSTEMMODEL_H

#include <memory>

#include "ModelBase.h"

class FileSystemModel
    : public ModelBase
{
    struct Private { };

public:
    FileSystemModel(TString tstrFullPath);
    FileSystemModel(Private) { }

    virtual const std::vector<ItemType> &GetChildrenSupportedItems() const;
    virtual TString GetItem(ItemType itemType) const;

    virtual bool Compare(const IModel &otherModel, ItemType itemType, bool isAscend) const;

protected:
    virtual void _UpdateInfo() const;
    virtual void _UpdateChildren() const;

private:
    static std::vector<ItemType> m_vType;

    static std::shared_ptr<FileSystemModel> __MakeShared();

};

#endif // FILESYSTEMMODEL_H
