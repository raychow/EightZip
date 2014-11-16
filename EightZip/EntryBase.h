// EntryBase.h

#ifndef ENTRYBASE_H
#define ENTRYBASE_H

#include <memory>

#include "SevenZipCore/TString.h"

#include "FileInfo.h"

template<typename T>
bool OrderCompare(const T &lhs, const T &rhs, bool isAscending)
{
    return isAscending ? (lhs < rhs) : (lhs > rhs);
}

class ModelBase;

enum class EntryItemType
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

class EntryBase
{
public:
    inline const TString &GetLocation() const { return m_tstrLocation; }

    inline const TString &GetName() const { return m_tstrName; }

    inline TString GetPath() const { return GetLocation() + GetName(); }

    inline const TString &GetType() const { return m_tstrType; }

    inline bool IsDirectory() const { return m_isDirectory; }

    inline bool IsVirtual() const { return m_isVirtual; }

    inline int GetIconIndex() const
    {
        if (-1 == m_nIconIndex)
        {
            m_nIconIndex = FileInfo::GetIconIndex(
                GetPath(), IsDirectory(), IsVirtual());
        }
        return m_nIconIndex;
    }

    virtual std::shared_ptr<ModelBase> GetContainer() const = 0;

    virtual std::shared_ptr<ModelBase> GetModel() const = 0;

    virtual bool IsOpenExternal() const;

    virtual bool CanExtract() const = 0;

    virtual void OpenExternal() const;

    virtual TString GetItem(EntryItemType itemType) const;

    virtual bool Compare(
        const EntryBase &otherEntry,
        EntryItemType itemType,
        bool isAscending) const;

    virtual ~EntryBase() = 0 { }

protected:
    EntryBase(TString tstrLocation,
        TString tstrName,
        TString tstrType,
        bool isDirectory,
        bool isVirtual);

    EntryBase(TString tstrLocation,
        TString tstrName,
        bool isDirectory,
        bool isVirtual);

    EntryBase(const EntryBase &) = delete;

private:
    TString m_tstrLocation;
    TString m_tstrName;
    TString m_tstrType;
    bool m_isDirectory;
    bool m_isVirtual;
    mutable int m_nIconIndex = -1;

};

#endif // ENTRYBASE_H
