// IModel.h

#ifndef IMODEL_H
#define IMODEL_H

#include <memory>
#include <vector>

#include <boost/optional.hpp>

#include "SevenZipCore/TString.h"

class IModel;

class IEntry
{
public:
    enum class ItemType
    {
        Name,
        Size,
        PackedSize,
        TotalSize,
        FreeSpace,
        Type,
        Modified,
        Created,
        Accessed,
        Attributes,
        Comment,
        Encrypted,
        Method,
        Block,
        Folders,
        Files,
        CRC
    };

    virtual const TString &GetName() const = 0;
    virtual const TString &GetFolder() const = 0;
    virtual TString GetPath() const = 0;
    virtual int GetIconIndex() const = 0;
    virtual TString GetItem(ItemType itemType) const = 0;
    virtual bool IsDirectory() const = 0;
    virtual std::shared_ptr<IModel> GetModel() = 0;
    virtual bool IsOpenExternal() const = 0;
    virtual void OpenExternal() = 0;
    virtual bool CanExtract() const = 0;

    virtual bool Compare(
        const IEntry &otherEntry,
        ItemType itemType,
        bool isAscending) const = 0;

    virtual ~IEntry() = 0 { }

};

class IModel
{
public:
    typedef std::vector<std::shared_ptr<IEntry>> EntryVector;

    virtual TString GetName() const = 0;
    virtual const TString &GetPath() const = 0;
    virtual bool HasParent() const = 0;
    virtual std::shared_ptr<IModel> GetParent() const = 0;
    virtual const EntryVector &GetEntries() const = 0;
    virtual const std::vector<IEntry::ItemType> &GetSupportedItems() const = 0;
    virtual bool IsArchive() const = 0;

    virtual ~IModel() = 0 { }

};

#endif // IMODEL_H
