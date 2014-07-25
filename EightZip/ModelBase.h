// ModelBase.h

#ifndef MODELBASE_H
#define MODELBASE_H

#include <memory>

#include "IModel.h"

struct ModelPrivate
{
    bool IsValid = false;
    bool IsChildrenValid = false;
    TString FileName;
    TString ParentFolder;
    wxIcon Icon;
    UINT64 Size = 0;
    UINT64 PackedSize = 0;
    wxDateTime Modified;
    wxDateTime Created;
    wxDateTime Accessed;
    boost::optional<UINT32> CRC;
    bool IsDirectory = false;
    bool IsOpenInside = false;
    IModel::ChildVector Children;
};

class ModelBase
    : public IModel
{
public:
    virtual const TString &GetName() const;
    virtual const TString &GetParentFolder() const;
    virtual TString GetFullPath() const;
    virtual const ChildVector &GetChildren() const;
    virtual bool IsDirectory() const;
    virtual bool IsOpenInside() const;
    virtual void Invalid() const;

protected:
    std::unique_ptr<ModelPrivate> m_upPrivate = std::unique_ptr<ModelPrivate>(new ModelPrivate);

    virtual void _UpdateInfo() const PURE;
    virtual void _UpdateChildren() const PURE;
};

#endif // MODELBASE_H
