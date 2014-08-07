// ModelBase.h

#ifndef MODELBASE_H
#define MODELBASE_H

#include <memory>
#include <mutex>

#include <boost/thread.hpp>

#include "IModel.h"

#define COMPARE(left, right, isAscending) \
    isAscending ? (left < right) : (left > right)

class EntryBase
    : public IEntry
{
public:
    virtual const TString &GetName() const { return m_tstrName; }
    virtual const TString &GetFolder() const { return m_tstrFolder; }
    virtual TString GetPath() const { return GetFolder() + GetName(); }
    virtual int GetIconIndex() const;
    TString GetItem(ItemType itemType) const;
    virtual bool IsDirectory() const { return m_isDirectory; }
    virtual bool IsOpenExternal() const;
    virtual void OpenExternal() const;

    virtual bool Compare(
        const IEntry &otherEntry, ItemType itemType, bool isAscending) const;

protected:
    // File name with extension.
    TString m_tstrName;
    // File path without filename, always with trailing separator.
    TString m_tstrFolder;
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

    static bool _LocaleCompare(
        const TString &tstrLeft, const TString & tstrRight, bool isAscending);

};

class ModelBase
    : public IModel
{
public:
    virtual const TString &GetPath() const { return m_tstrPath; }
    virtual bool HasParent() const { return !GetPath().empty(); }
    virtual const EntryVector &GetEntries() const { return m_vspEntry; }

protected:
    // File path without filename, always with trailing separator.
    TString m_tstrPath;

    IModel::EntryVector m_vspEntry;

};

#endif // MODELBASE_H
