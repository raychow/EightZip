// FileSystemModel.h

#ifndef FILESYSTEMMODEL_H
#define FILESYSTEMMODEL_H

#include <memory>

#include "ModelBase.h"

class FileSystemEntry
    : public EntryBase
{
public:
    FileSystemEntry(TString tstrPath
        , TString tstrName
        , bool isDirectory
        , wxULongLong_t un64Size
        , wxDateTime dtAccessed
        , wxDateTime dtModified
        , wxDateTime dtCreated
        , TString tstrType);

    virtual std::shared_ptr<IModel> GetModel() const;
};

class FileSystemModel
    : public ModelBase
{
public:
    FileSystemModel(TString tstrFullPath);

    virtual std::shared_ptr<IModel> GetParent() const;
    virtual const std::vector<IEntry::ItemType> &GetSupportedItems() const;

private:
    static std::vector<IEntry::ItemType> m_vType;

};

#endif // FILESYSTEMMODEL_H
