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

Extractor &Extractor::AddPlan(const EntryBase &entry)
{
    if (entry.IsVirtual())
    {
        auto archiveFile = *dynamic_cast<const VirtualEntry &>(
            entry).GetArchiveFile();
        auto &plan = m_plans[*archiveFile.GetArchiveEntry()];
        plan.push_back(
            archiveFile.GetIndex());
        if (entry.IsDirectory())
        {
            auto model = dynamic_cast<const VirtualModel &>(*entry.GetModel());
            queue<const SevenZipCore::ArchiveFolder *> qpFolder;
            qpFolder.push(model.GetArchiveFolder().get());

            while (!qpFolder.empty())
            {
                const auto &folder = *qpFolder.front();
                qpFolder.pop();
                for (const auto &spFile : folder.GetFiles())
                {
                    plan.push_back(spFile->GetIndex());
                }
                for (const auto &spFolder : folder.GetFolders())
                {
                    auto un32ArchiveIndex = spFolder->GetIndex();
                    if (UINT32_MAX != un32ArchiveIndex)
                    {
                        plan.push_back(un32ArchiveIndex);
                    }
                    qpFolder.push(spFolder.get());
                }
            }

        }
    }
    else
    {
        m_entries.push_back(entry);
    }
    return *this;
}

Extractor &Extractor::AddPlan(const VirtualModel &model)
{
    auto &archiveEntry = *model.GetArchive()->GetArchiveEntry();
    if (model.IsRoot())
    {
        m_plans[archiveEntry].clear();
    }
    else
    {
        bool isExist = m_plans.count(archiveEntry) > 0;
        auto &plan = m_plans[archiveEntry];
        if (!isExist || !plan.empty())
        {
            auto vun32Index = model.GetArchiveFolder()->GetAllIndexes();
            plan.insert(plan.end(), vun32Index.cbegin(), vun32Index.cend());
        }
    }
    return *this;
}

Extractor &Extractor::Execute()
{
    for (auto &plan : m_plans)
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
    for (const auto &spEntry : m_entries)
    {
        if (m_pProgressDialog && m_pProgressDialog->IsCancelled())
        {
            return *this;
        }
        __ExtractFile(spEntry);
    }

    return *this;
}

void Extractor::__ExtractFile(const EntryBase &entry)
{
    bool isSuccess = false;
    try
    {
        auto model = dynamic_cast<const VirtualModel &>(
            *entry.GetModel());
        __Execute(*model.GetArchive()->GetArchiveEntry(), vector < UINT32 > {});
        isSuccess = true;
    }
    catch (const bad_cast &)
    {
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
                entry.GetPath()).ToStdWstring());
        }
    }
}

void Extractor::__Execute(const SevenZipCore::ArchiveEntry &archiveEntry,
    std::vector<UINT32> &vun32ArchiveIndex)
{
    auto spArchive = archiveEntry.GetArchive();
    if (m_pProgressDialog)
    {
        m_pProgressDialog->SetArchivePath(spArchive->GetPath());
    }
    try
    {
        auto inArchiveAdapter = SevenZipCore::IInArchiveAdapter<>(
            archiveEntry.GetInArchive());
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
