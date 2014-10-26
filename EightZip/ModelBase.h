// ModelBase.h

#ifndef MODELBASE_H
#define MODELBASE_H

#include <memory>
#include <mutex>

#include <boost/thread.hpp>

#include "SevenZipCore/CommonHelper.h"
#include "SevenZipCore/TString.h"

enum class EntryItemType;
class EntryBase;

class ModelBase
{
public:
    typedef std::vector<std::shared_ptr<EntryBase>> EntryVector;
    typedef EntryVector::iterator iterator;
    typedef EntryVector::const_iterator const_iterator;
    typedef EntryVector::reverse_iterator reverse_iterator;
    typedef EntryVector::const_reverse_iterator const_reverse_iterator;

    inline const TString &GetLocation() const { return m_tstrLocation; }
    inline void SetLocation(TString tstrLocation)
    {
        m_tstrLocation = SevenZipCore::Helper::MakePathSlash(move(tstrLocation));
    }
    
    inline const TString &GetName() const { return m_tstrName; }
    inline void SetName(TString tstrName) { m_tstrName = move(tstrName); }

    inline const TString GetPath() const
    {
        return m_tstrLocation + m_tstrName;
    }

    inline bool HasParent() const { return !m_tstrName.empty(); }

    inline std::shared_ptr<EntryBase> GetEntry(int index) const
    {
        __Initialize();
        return m_vspEntry.at(index);
    }

    virtual std::shared_ptr<ModelBase> GetParent() const = 0;

    virtual const std::vector<EntryItemType> &GetSupportedItems() const = 0;

    virtual bool IsArchive() const = 0;

    inline std::shared_ptr<EntryBase> operator[](int index) const
    {
        __Initialize();
        return m_vspEntry[index];
    }

    inline iterator begin()
    {
        __Initialize();
        return m_vspEntry.begin();
    }
    inline iterator end() const
    {
        __Initialize();
        return m_vspEntry.end();
    }
    inline reverse_iterator rbegin()
    {
        __Initialize();
        return m_vspEntry.rbegin();
    }
    inline reverse_iterator rend() const
    {
        __Initialize();
        return m_vspEntry.rend();
    }
    inline const_iterator cbegin() const
    {
        __Initialize();
        return m_vspEntry.cbegin();
    }
    inline const_iterator cend() const
    {
        __Initialize();
        return m_vspEntry.cend();
    }
    inline const_reverse_iterator crbegin() const
    {
        __Initialize();
        return m_vspEntry.crbegin();
    }
    inline const_reverse_iterator crend() const
    {
        __Initialize();
        return m_vspEntry.crend();
    }
    inline EntryVector::size_type GetEntryCount() const
    {
        __Initialize();
        return m_vspEntry.size();
    }

protected:
    ModelBase(TString tstrLocation, TString tstrName);
    virtual ~ModelBase() = 0 { }

    virtual EntryVector _InitializeEntries() const = 0;

private:
    TString m_tstrLocation;
    TString m_tstrName;

    mutable bool m_isInitialized = false;
    mutable EntryVector m_vspEntry;

    inline void __Initialize() const
    {
        if (!m_isInitialized)
        {
            m_vspEntry = _InitializeEntries();
            m_isInitialized = true;
        }
    }
    
};

std::shared_ptr<ModelBase> GetModelFromPath(
    TString tstrPath, bool isTryOpenArchive = true);
std::shared_ptr<ModelBase> GetModelFromPath(
    std::shared_ptr<ModelBase> spModel, TString tstrPath);

#endif // MODELBASE_H
