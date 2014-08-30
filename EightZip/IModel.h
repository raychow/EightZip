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

    virtual const TString &GetName() const PURE;
    virtual const TString &GetFolder() const PURE;
    virtual TString GetPath() const PURE;
    virtual int GetIconIndex() const PURE;
    virtual TString GetItem(ItemType itemType) const PURE;
    virtual bool IsDirectory() const PURE;
    virtual std::shared_ptr<IModel> GetModel() PURE;
    virtual bool IsOpenExternal() const PURE;
    virtual void OpenExternal() PURE;
    virtual bool CanExtract() const PURE;

    virtual bool Compare(
        const IEntry &otherEntry,
        ItemType itemType,
        bool isAscending) const PURE;
};

class IModel
{
public:
    typedef std::vector<std::shared_ptr<IEntry>> EntryVector;

    virtual const TString &GetPath() const PURE;
    //virtual TString GetParentPath() const PURE;
    virtual bool HasParent() const PURE;
    virtual bool IsParentArchive() const PURE;
    virtual std::shared_ptr<IModel> GetParent() const PURE;
    virtual const EntryVector &GetEntries() const PURE;
    virtual const std::vector<IEntry::ItemType> &GetSupportedItems() const PURE;
    virtual bool IsArchive() const PURE;

};

#endif // IMODEL_H
