// FileSystemModel.h

#ifndef FILESYSTEMMODEL_H
#define FILESYSTEMMODEL_H

#include <memory>

#include "ModelBase.h"

class FileSystemModel
    : public ModelBase
{
public:
    FileSystemModel(TString tstrPath);

    virtual const std::vector<ItemType> &GetChildrenSupportedItems() const;
    virtual const TString GetItem(ItemType itemType) const;

protected:
    virtual void _UpdateInfo() const;
    virtual void _UpdateChildren() const;

private:
    static wxIcon __GetFileIcon(TString tstrExtension, bool isFolder, bool isSmallIcon);
    static std::vector<ItemType> m_vType;

};

#endif // FILESYSTEMMODEL_H
