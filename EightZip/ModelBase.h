// ModelBase.h

#ifndef MODELBASE_H
#define MODELBASE_H

#include <memory>
#include <mutex>
#include <vector>

#include <boost/optional.hpp>
#include <boost/thread.hpp>

#include "SevenZipCore/TString.h"

#include "FileInfo.h"

template<typename T>
bool OrderCompare(const T &lhs, const T &rhs, bool isAscending)
{
    return isAscending ? (lhs < rhs) : (lhs > rhs);
}

enum class ModelItemType
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

class ModelBase
{
public:
    typedef std::vector<std::shared_ptr<ModelBase>> Models;

    inline TString GetLocation() const { return m_tstrLocation; }

    inline void SetLocation(TString tstrLocation)
    {
        m_tstrLocation = move(tstrLocation);
    }

    inline TString GetFileName() const { return m_tstrFileName; }

    inline void SetFileName(TString tstrFileName)
    {
        m_tstrFileName = move(tstrFileName);
    }

    inline const TString GetPath() const
    {
        return m_tstrLocation + m_tstrFileName;
    }

    inline const TString &GetType() const { return m_tstrType; }

    inline void SetType(TString tstrType) { m_tstrType = tstrType; }

    inline bool IsDirectory() const { return m_isDirectory; }

    inline bool IsVirtual() const { return m_isVirtual; }

    inline bool HasParent() const
    {
        return !m_tstrLocation.empty();
    }

    inline int GetIconIndex() const
    {
        if (-1 == m_nIconIndex)
        {
            m_nIconIndex = FileInfo::GetIconIndex(
                GetPath(), IsDirectory(), IsVirtual());
        }
        return m_nIconIndex;
    }

    virtual std::shared_ptr<ModelBase> GetParent() const = 0;

    virtual const std::vector<ModelItemType> &GetSupportedItems() const = 0;

    virtual bool CanExtract() const = 0;

    virtual const Models GetChildren() const = 0;

    virtual TString GetItem(ModelItemType itemType) const;

    virtual void OpenExternal() const;

    virtual bool Compare(const ModelBase &otherModel,
        ModelItemType itemType,
        bool isAscending) const;

    virtual ~ModelBase() = 0 { }

protected:
    ModelBase(TString tstrLocation,
        TString tstrFileName,
        TString tstrType,
        bool isDirectory,
        bool isVirtual);

    ModelBase(TString tstrLocation,
        TString tstrFileName,
        bool isDirectory,
        bool isVirtual);

private:
    TString m_tstrLocation;
    TString m_tstrFileName;
    TString m_tstrType;
    bool m_isDirectory;
    bool m_isVirtual;
    mutable int m_nIconIndex = -1;

};

std::shared_ptr<ModelBase> GetModelFromPath(
    TString tstrPath, bool isTryOpenArchive = true);
std::shared_ptr<ModelBase> GetModelFromPath(
    std::shared_ptr<ModelBase> spModel, TString tstrPath);

#endif // MODELBASE_H
