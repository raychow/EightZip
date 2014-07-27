// ModelBase.h

#ifndef MODELBASE_H
#define MODELBASE_H

#include <memory>
#include <mutex>

#include <boost/thread.hpp>

#include "IModel.h"

#define COMPARE(left, right, isAscending) isAscending ? (left < right) : (left > right)

class ModelBase
    : public IModel
{
public:
    virtual const TString &GetName() const { return m_tstrName; }
    virtual const TString &GetPath() const { return m_tstrPath; }
    virtual TString GetFullPath() const { return GetPath() + GetName(); }
    virtual bool HasParent() const { return !GetName().empty(); }
    virtual const ChildVector &GetChildren()
    {
        _UpdateChildren();
        return m_vspChild;
    }
    virtual int GetIconIndex() const;
    TString GetItem(ItemType itemType) const;
    virtual bool IsDirectory() const { return m_isDirectory; }
    virtual bool IsOpenInside() const { return m_isOpenInside; }

    virtual bool Compare(const IModel &otherModel, ItemType itemType, bool isAscending) const;

protected:
    bool m_isChildrenValid = false;
    // File path without filename, always with trailing separator.
    TString m_tstrPath;
    // File name with extension.
    TString m_tstrName;
    mutable int m_nIconIndex = -1;
    wxULongLong_t m_un64Size = 0;
    wxULongLong_t m_un64PackedSize = 0;
    wxULongLong_t m_un64TotalSize = 0;
    wxULongLong_t m_un64FreeSpace = 0;
    TString m_tstrType;
    wxDateTime m_dtModified;
    wxDateTime m_dtCreated;
    wxDateTime m_dtAccessed;
    boost::optional<UINT32> m_oun32CRC;
    bool m_isDirectory = false;
    bool m_isOpenInside = false;
    IModel::ChildVector m_vspChild;

    virtual void _UpdateChildren() PURE;

    static bool _LocaleCompare(const TString &tstrLeft, const TString & tstrRight, bool isAscending);

};

#endif // MODELBASE_H
