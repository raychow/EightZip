// ModelBase.h

#ifndef MODELBASE_H
#define MODELBASE_H

#include <memory>

#include "IModel.h"

struct ModelPrivate
{
    bool IsValid = false;
    bool IsChildrenValid = false;
    TString Name;
    TString FullPath;
    wxIcon Icon;
    UINT64 Size = 0;
    UINT64 PackedSize = 0;
    // ...
    boost::optional<UINT32> CRC;
    bool IsFolder = false;
    bool IsOpenInside = false;
    IModel::ChildVector DirChildren;
    IModel::ChildVector FileChidren;
};

class ModelBase
    : public IModel
{
public:
    virtual const TString &GetName() const;
    virtual const TString &GetFullPath() const;
    virtual const ChildVector &GetDirChildren() const;
    virtual const ChildVector &GetFileChildren() const;
    virtual bool IsFolder() const;
    virtual bool IsOpenInside() const;
    virtual void Invalid() const;

protected:
    std::unique_ptr<ModelPrivate> m_upPrivate = std::unique_ptr<ModelPrivate>(new ModelPrivate);

    virtual void _UpdateInfo() const PURE;
    virtual void _UpdateChildren() const PURE;
};

#endif // MODELBASE_H
