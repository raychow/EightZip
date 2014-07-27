// ModelBase.h

#ifndef MODELBASE_H
#define MODELBASE_H

#include <memory>
#include <mutex>

#include <boost/thread.hpp>

#include "IModel.h"

struct ModelPrivate
{
    bool IsValid = false;
    bool IsChildrenValid = false;
    // File path without filename, always with trailing separator.
    TString Path;
    // File name with extension.
    TString Name;
    int IconIndex = -1;
    wxULongLong_t Size = 0;
    wxULongLong_t PackedSize = 0;
	TString Type;
    wxDateTime Modified;
    wxDateTime Created;
    wxDateTime Accessed;
    boost::optional<UINT32> CRC;
    bool IsDirectory = false;
    bool IsOpenInside = false;
    IModel::ChildVector Children;

	std::mutex UpdateMutex;
	boost::thread UpdateThread;
};

class ModelBase
    : public IModel
{
public:
    virtual const TString &GetName() const;
    virtual const TString &GetPath() const;
    virtual TString GetFullPath() const;
	virtual const ChildVector &GetChildren() const;
	virtual int GetIconIndex() const;
	TString GetItem(ItemType itemType) const;
	virtual bool IsDirectory() const;
    virtual bool IsOpenInside() const;
    virtual void Invalid() const;

protected:
    std::unique_ptr<ModelPrivate> m_upPrivate = std::unique_ptr<ModelPrivate>(new ModelPrivate);

    virtual void _UpdateChildren() const PURE;

};

#endif // MODELBASE_H
