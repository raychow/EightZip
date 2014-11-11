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

class Extractor
{
public:
    Extractor(
        TString tstrPath, SevenZipCore::IExtractIndicator *pExtractIndicator);

    void AddPlan(std::shared_ptr<SevenZipCore::ArchiveEntry> spArchiveEntry);
    void AddPlan(std::shared_ptr<SevenZipCore::ArchiveEntry> spArchiveEntry,
        UINT32 un32ArchiveIndex);
    void AddPlan(std::shared_ptr<SevenZipCore::ArchiveEntry> spArchiveEntry,
        std::vector<UINT32> vun32ArchiveIndex);

    inline void SetInternalPath(TString tstrInternalPath)
    {
        m_tstrInternalPath = tstrInternalPath;
    }

    void Execute() const;

private:
    TString m_tstrPath;
    TString m_tstrInternalPath;
    SevenZipCore::IExtractIndicator *m_pExtractIndicator = nullptr;
    std::map<std::shared_ptr<SevenZipCore::ArchiveEntry>,
        std::vector<UINT32>> m_plans;

    Extractor(const Extractor&) = delete;
    Extractor &operator=(const Extractor &) = delete;

};

#endif // EXTRACTOR_H
