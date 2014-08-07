// FolderModel.h

#ifndef FILESYSTEMMODEL_H
#define FILESYSTEMMODEL_H

#include <memory>

#include "ModelBase.h"

class FolderEntry
    : public EntryBase
{
public:
    FolderEntry(TString tstrPath,
        TString tstrName,
        bool isDirectory,
        wxULongLong_t un64Size,
        wxDateTime dtAccessed,
        wxDateTime dtModified,
        wxDateTime dtCreated);

    virtual bool IsOpenDirectly() const;
    virtual std::shared_ptr<IModel> GetModel();

};

class FolderModel
    : public ModelBase
{
public:
    FolderModel(TString tstrPath);

    virtual std::shared_ptr<IModel> GetParent() const;
    virtual const std::vector<IEntry::ItemType> &GetSupportedItems() const;

};

#endif // FILESYSTEMMODEL_H
