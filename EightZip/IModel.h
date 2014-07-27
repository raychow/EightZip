// IModel.h

#ifndef IMODEL_H
#define IMODEL_H

#include <memory>
#include <vector>

#include <boost/optional.hpp>

#include "SevenZipCore/TString.h"

class IModel
{
public:
    typedef std::vector<std::shared_ptr<IModel>> ChildVector;

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
    virtual const TString &GetPath() const PURE;
    virtual TString GetFullPath() const PURE;
    virtual bool HasParent() const PURE;
    virtual std::shared_ptr<IModel> GetParent() const PURE;
    virtual const ChildVector &GetChildren() PURE;
    virtual const std::vector<ItemType> &GetChildrenSupportedItems() const PURE;
    virtual int GetIconIndex() const PURE;
    virtual TString GetItem(ItemType itemType) const PURE;
    virtual bool IsDirectory() const PURE;
    virtual bool IsOpenInside() const PURE;

    virtual bool Compare(const IModel &otherModel, ItemType itemType, bool isAscending) const PURE;
};

#endif // IMODEL_H
