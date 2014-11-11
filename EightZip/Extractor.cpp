#include "stdwx.h"
#include "Extractor.h"

#include "SevenZipCore/ArchiveEntry.h"
#include "SevenZipCore/ArchiveExtractCallback.h"
#include "SevenZipCore/ComPtr.h"
#include "SevenZipCore/Exception.h"
#include "SevenZipCore/IExtractIndicator.h"

using namespace std;

shared_ptr<SevenZipCore::ArchiveExtractCallback> CreateCallback(
    shared_ptr<SevenZipCore::Archive> spArchive,
    TString tstrPath, TString tstrInternalPath,
    SevenZipCore::IExtractIndicator *pExtractIndicator)
{
    return SevenZipCore::MakeComPtr(
        new SevenZipCore::ArchiveExtractCallback(
        spArchive,
        false,
        false,
        false,
        move(tstrPath),
        move(tstrInternalPath),
        SevenZipCore::ExtractPathMode::CurrentPathNames,
        SevenZipCore::ExtractOverwriteMode::AskBefore,
        pExtractIndicator));
}


Extractor::Extractor(
    TString tstrPath, SevenZipCore::IExtractIndicator *pExtractIndicator)
    : m_tstrPath(tstrPath)
    , m_pExtractIndicator(pExtractIndicator)
{

}

Extractor &Extractor::AddPlan(
    shared_ptr<SevenZipCore::ArchiveEntry> spArchiveEntry,
    vector<UINT32> vun32ArchiveIndex)
{
    if (vun32ArchiveIndex.empty())
    {
        return *this;
    }
    auto &indexes = m_plans[spArchiveEntry];
    indexes.insert(indexes.end(),
        vun32ArchiveIndex.cbegin(), vun32ArchiveIndex.cend());
    return *this;
}

Extractor &Extractor::Execute()
{
    for (const auto &plan : m_plans)
    {
        auto spArchive = plan.first->GetArchive();
        try
        {
            auto inArchiveAdapter = SevenZipCore::IInArchiveAdapter<>(
                plan.first->GetInArchive());
            auto cpCallback = CreateCallback(
                spArchive,
                m_tstrPath,
                m_tstrInternalLocation,
                m_pExtractIndicator);
            if (plan.second.empty())
            {
                inArchiveAdapter.ExtractAll(
                    false,
                    cpCallback.get());
            }
            else
            {
                inArchiveAdapter.Extract(plan.second, false, cpCallback.get());
            }
            m_tstrLastExtractPath = cpCallback->GetLastExtractPath();
        }
        catch (const SevenZipCore::ArchiveException &)
        {
            if (m_pExtractIndicator)
            {
                m_pExtractIndicator->AddError(wxString::Format(
                    _("Cannot extract \"%s\"."),
                    spArchive->GetPath()).ToStdWstring());
            }
        }
    }
    return *this;
}
