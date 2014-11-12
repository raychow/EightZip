// Extractor.h

#ifndef EXTRACTOR_H
#define EXTRACTOR_H

#include <map>
#include <memory>
#include <vector>

#include "SevenZipCore/BaseType.h"
#include "SevenZipCore/TString.h"

namespace SevenZipCore
{
    class ArchiveEntry;
    class IExtractIndicator;
}

class EntryBase;
class VirtualModel;

class Extractor
{
public:
    Extractor(
        TString tstrPath, SevenZipCore::IExtractIndicator *pExtractIndicator);

    inline Extractor &AddPlan(
        std::shared_ptr<SevenZipCore::ArchiveEntry> spArchiveEntry)
    {
        m_plans[spArchiveEntry].clear();
        return *this;
    }
    inline Extractor &AddPlan(
        std::shared_ptr<SevenZipCore::ArchiveEntry> spArchiveEntry,
        UINT32 un32ArchiveIndex)
    {
        m_plans[spArchiveEntry].push_back(un32ArchiveIndex);
        return *this;
    }
    Extractor &AddPlan(std::shared_ptr<SevenZipCore::ArchiveEntry> spArchiveEntry,
        std::vector<UINT32> vun32ArchiveIndex);
    Extractor &AddPlan(std::shared_ptr<EntryBase> spEntry);
    Extractor &AddPlan(std::shared_ptr<VirtualModel> spModel);

    inline Extractor &SetInternalLocation(TString tstrInternalLocation)
    {
        m_tstrInternalLocation = tstrInternalLocation;
        return *this;
    }

    inline const TString &GetLastExtractPath() const
    {
        return m_tstrLastExtractPath;
    }

    Extractor &Execute();

private:
    TString m_tstrPath;
    TString m_tstrInternalLocation;
    mutable TString m_tstrLastExtractPath;
    SevenZipCore::IExtractIndicator *m_pExtractIndicator = nullptr;
    std::map<std::shared_ptr<SevenZipCore::ArchiveEntry>,
        std::vector<UINT32>> m_plans;
    std::vector<std::shared_ptr<EntryBase>> m_entries;

    Extractor(const Extractor&) = delete;
    Extractor &operator=(const Extractor &) = delete;

};

#endif // EXTRACTOR_H
