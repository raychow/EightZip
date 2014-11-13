#include "stdwx.h"
#include "Extractor.h"

#include <queue>

#include "SevenZipCore/ArchiveEntry.h"
#include "SevenZipCore/ArchiveExtractCallback.h"
#include "SevenZipCore/ArchiveFile.h"
#include "SevenZipCore/ComPtr.h"
#include "SevenZipCore/Exception.h"

#include "EntryBase.h"
#include "ExtractIndicator.h"
#include "ProgressDialog.h"
#include "VirtualEntry.h"
#include "VirtualModel.h"

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
    TString tstrPath, ExtractIndicator *pExtractIndicator)
    : m_tstrPath(tstrPath)
    , m_pExtractIndicator(pExtractIndicator)
    , m_pProgressDialog(pExtractIndicator
    ? pExtractIndicator->GetProgressDialog() : nullptr)
{

}

Extractor &Extractor::AddPlan(shared_ptr<const EntryBase> spEntry)
{
    m_entries.push_back(move(spEntry));
    return *this;
}

Extractor &Extractor::AddPlan(shared_ptr<const VirtualModel> spModel)
{
    m_models.push_back(move(spModel));
    return *this;
}

Extractor &Extractor::Execute()
{
    map<shared_ptr<const SevenZipCore::ArchiveEntry>, vector<UINT32>> plans;
    for (const auto &spEntry : m_entries)
    {
        if (spEntry->IsVirtual())
        {
            auto spArchiveFile = dynamic_pointer_cast<const VirtualEntry>(
                spEntry)->GetArchiveFile();
            plans[spArchiveFile->GetArchiveEntry()].push_back(
                spArchiveFile->GetIndex());
            if (spEntry->IsDirectory())
            {
                m_models.push_back(
                    dynamic_pointer_cast<VirtualModel>(spEntry->GetModel()));
            }
        }
        else
        {
            if (m_pProgressDialog && m_pProgressDialog->IsCancelled())
            {
                return *this;
            }
            __ExtractFile(spEntry);
        }
    }
    for (const auto &spModel : m_models)
    {
        auto spArchiveEntry = spModel->GetArchive()->GetArchiveEntry();
        if (spModel->IsRoot())
        {
            plans[spArchiveEntry].clear();
        }
        else
        {
            bool isExist = plans.count(spArchiveEntry) > 0;
            auto &plan = plans[spArchiveEntry];
            if (isExist && plan.empty())
            {
                continue;
            }
            auto vun32Index = spModel->GetArchiveFolder()->GetAllIndexes();
            plan.insert(plan.end(), vun32Index.cbegin(), vun32Index.cend());
        }
    }

    for (auto &plan : plans)
    {
        if (m_pProgressDialog && m_pProgressDialog->IsCancelled())
        {
            return *this;
        }
        sort(plan.second.begin(), plan.second.end());
        plan.second.erase(unique(plan.second.begin(), plan.second.end()),
            plan.second.end());
        __Execute(plan.first, plan.second);
    }

    return *this;
}

void Extractor::__ExtractFile(shared_ptr<const EntryBase> spEntry)
{
    bool isSuccess = false;
    try
    {
        auto spModel = dynamic_pointer_cast<VirtualModel>(
            spEntry->GetModel());
        if (spModel)
        {
            __Execute(spModel->GetArchive()->GetArchiveEntry(),
                vector < UINT32 > {});
            isSuccess = true;
        }
    }
    catch (const SevenZipCore::ArchiveException &)
    {
    }
    if (!isSuccess)
    {
        if (m_pExtractIndicator)
        {
            m_pExtractIndicator->AddError(wxString::Format(
                _("%s: The archive is either in unknown format or damaged."),
                spEntry->GetPath()).ToStdWstring());
        }
    }
}

void Extractor::__Execute(shared_ptr<const SevenZipCore::ArchiveEntry> spArchiveEntry,
    vector<UINT32> &vun32ArchiveIndex)
{
    auto spArchive = spArchiveEntry->GetArchive();
    try
    {
        auto inArchiveAdapter = SevenZipCore::IInArchiveAdapter<>(
            spArchiveEntry->GetInArchive());
        auto cpCallback = CreateCallback(
            spArchive,
            m_tstrPath,
            m_tstrInternalLocation,
            m_pExtractIndicator);
        if (vun32ArchiveIndex.empty())
        {
            inArchiveAdapter.ExtractAll(
                false,
                cpCallback.get());
        }
        else
        {
            sort(vun32ArchiveIndex.begin(), vun32ArchiveIndex.end());
            inArchiveAdapter.Extract(vun32ArchiveIndex, false, cpCallback.get());
        }
        m_tstrLastExtractPath = cpCallback->GetLastExtractPath();
    }
    catch (const SevenZipCore::ArchiveException &)
    {
        if (m_pExtractIndicator
            && m_pProgressDialog && !m_pProgressDialog->IsCancelled())
        {
            m_pExtractIndicator->AddError(wxString::Format(
                _("Cannot extract \"%s\"."),
                spArchive->GetPath()).ToStdWstring());
        }
    }
}
