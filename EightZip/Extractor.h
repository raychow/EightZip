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
}

class EntryBase;
class ExtractIndicator;
class ProgressDialog;
class VirtualModel;

class ArchiveEntryComparer
{
public:
    bool operator()(const SevenZipCore::ArchiveEntry &lhs,
        const SevenZipCore::ArchiveEntry &rhs)
    {
        return &lhs < &rhs;
    }
};

class Extractor
{
public:
    Extractor(
        TString tstrPath, ExtractIndicator *pExtractIndicator);

    Extractor &AddPlan(const EntryBase &entry);
    Extractor &AddPlan(const VirtualModel &model);

    inline Extractor &SetInternalLocation(TString tstrInternalLocation)
    {
        m_tstrInternalLocation = move(tstrInternalLocation);
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
    TString m_tstrLastExtractPath;
    ExtractIndicator *m_pExtractIndicator = nullptr;
    ProgressDialog *m_pProgressDialog = nullptr;
    std::map<std::reference_wrapper<const SevenZipCore::ArchiveEntry>,
        std::vector<UINT32>, ArchiveEntryComparer> m_plans;
    std::vector<std::reference_wrapper<const EntryBase>> m_entries;
    Extractor(const Extractor&) = delete;
    Extractor &operator=(const Extractor &) = delete;

    void __ExtractFile(const EntryBase &entry);
    void __Execute(const SevenZipCore::ArchiveEntry &archiveEntry,
        std::vector<UINT32> &vun32ArchiveIndex);

};

#endif // EXTRACTOR_H
