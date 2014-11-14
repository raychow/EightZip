// ModelBase.h

#ifndef MODELBASE_H
#define MODELBASE_H

#include <memory>
#include <mutex>
#include <vector>

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

    inline bool HasParent() const
    {
        return !(m_tstrName.empty() &&
            m_tstrLocation.size() == 1 && wxIsPathSeparator(m_tstrLocation[0]));
    }

    inline EntryBase &GetEntry(EntryVector::size_type index) const
    {
        __Initialize();
        return *m_vupEntry.at(index);
    }

    virtual std::shared_ptr<ModelBase> GetParent() const = 0;

    virtual const std::vector<EntryItemType> &GetSupportedItems() const = 0;

    virtual bool IsArchive() const = 0;

    inline EntryBase &operator[](EntryVector::size_type index) const
    {
        __Initialize();
        return *m_vupEntry[index];
    }

    inline iterator begin()
    {
        __Initialize();
        return m_vupEntry.begin();
    }
    inline const_iterator begin() const
    {
        __Initialize();
        return m_vupEntry.begin();
    }
    inline iterator end()
    {
        __Initialize();
        return m_vupEntry.end();
    }
    inline const_iterator end() const
    {
        __Initialize();
        return m_vupEntry.end();
    }
    inline reverse_iterator rbegin()
    {
        __Initialize();
        return m_vupEntry.rbegin();
    }
    inline reverse_iterator rend() const
    {
        __Initialize();
        return m_vupEntry.rend();
    }
    inline const_iterator cbegin() const
    {
        __Initialize();
        return m_vupEntry.cbegin();
    }
    inline const_iterator cend() const
    {
        __Initialize();
        return m_vupEntry.cend();
    }
    inline const_reverse_iterator crbegin() const
    {
        __Initialize();
        return m_vupEntry.crbegin();
    }
    inline const_reverse_iterator crend() const
    {
        __Initialize();
        return m_vupEntry.crend();
    }
    inline EntryVector::size_type GetEntryCount() const
    {
        __Initialize();
        return m_vupEntry.size();
    }

    virtual ~ModelBase() = 0 { }

protected:
    ModelBase(TString tstrLocation, TString tstrName);
    ModelBase(const ModelBase &) = default;

    virtual EntryVector _InitializeEntries() const = 0;

private:
    TString m_tstrLocation;
    TString m_tstrName;

    mutable bool m_isInitialized = false;
    mutable EntryVector m_vupEntry;

    inline void __Initialize() const
    {
        if (!m_isInitialized)
        {
            m_vupEntry = _InitializeEntries();
            m_isInitialized = true;
        }
    }
    
};

std::shared_ptr<ModelBase> GetModelFromPath(
    TString tstrPath, bool isTryOpenArchive = true);
std::shared_ptr<ModelBase> GetModelFromPath(
    std::shared_ptr<ModelBase> spModel, TString tstrPath);

#endif // MODELBASE_H
