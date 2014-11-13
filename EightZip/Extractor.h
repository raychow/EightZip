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

class Extractor
{
public:
    Extractor(
        TString tstrPath, ExtractIndicator *pExtractIndicator);

    Extractor &AddPlan(std::shared_ptr<const EntryBase> spEntry);
    Extractor &AddPlan(std::shared_ptr<const VirtualModel> spModel);

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
    TString m_tstrLastExtractPath;
    ExtractIndicator *m_pExtractIndicator = nullptr;
    ProgressDialog *m_pProgressDialog = nullptr;
    std::map<std::shared_ptr<SevenZipCore::ArchiveEntry>,
        std::vector<UINT32>> m_plans;
    std::vector<std::shared_ptr<const VirtualModel>> m_models;
    std::vector<std::shared_ptr<const EntryBase>> m_entries;
    Extractor(const Extractor&) = delete;
    Extractor &operator=(const Extractor &) = delete;

    void __ExtractFile(std::shared_ptr<const EntryBase> spEntry);
    void __Execute(std::shared_ptr<const SevenZipCore::ArchiveEntry> spArchiveEntry,
        std::vector<UINT32> &vun32ArchiveIndex);

};

#endif // EXTRACTOR_H
