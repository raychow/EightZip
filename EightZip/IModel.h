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
        Size,
        PackedSize,
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
    virtual const TString &GetFullPath() const PURE;
    virtual const ChildVector &GetDirChildren() const PURE;
    virtual const ChildVector &GetFileChildren() const PURE;
    virtual const std::vector<ItemType> &GetChildrenSupportedItems() const PURE;
    virtual const TString GetItem(ItemType itemType) const PURE;
    virtual bool IsFolder() const PURE;
    virtual bool IsOpenInside() const PURE;
};

#endif // IMODEL_H
