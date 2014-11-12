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

Extractor &Extractor::AddPlan(shared_ptr<EntryBase> spEntry)
{
    if (spEntry->IsVirtual())
    {
        auto spArchiveFile = dynamic_pointer_cast<VirtualEntry>(
            spEntry)->GetArchiveFile();
        AddPlan(spArchiveFile->GetArchiveEntry(), spArchiveFile->GetIndex());
        if (spEntry->IsDirectory())
        {
            AddPlan(dynamic_pointer_cast<VirtualModel>(spEntry->GetModel()));
        }
    }
    else
    {
        m_entries.push_back(spEntry);
    }
    return *this;
}

Extractor &Extractor::AddPlan(shared_ptr<VirtualModel> spModel)
{
    auto spArchiveEntry = spModel->GetArchiveFolder()->GetArchiveEntry();
    if (spModel->IsRoot())
    {
        return AddPlan(move(spArchiveEntry));
    }
    else
    {
        vector<UINT32> vun32ArchiveIndex;
        queue<const SevenZipCore::ArchiveFolder *> qpFolder;
        qpFolder.push(spModel->GetArchiveFolder().get());

        while (!qpFolder.empty())
        {
            const auto &folder = *qpFolder.front();
            qpFolder.pop();
            for (const auto &spFile : folder.GetFiles())
            {
                vun32ArchiveIndex.push_back(spFile->GetIndex());
            }
            for (const auto &spFolder : folder.GetFolders())
            {
                auto un32ArchiveIndex = spFolder->GetIndex();
                if (UINT32_MAX != un32ArchiveIndex)
                {
                    vun32ArchiveIndex.push_back(un32ArchiveIndex);
                }
                qpFolder.push(spFolder.get());
            }
        }
        return AddPlan(move(spArchiveEntry), move(vun32ArchiveIndex));
    }
}

Extractor &Extractor::Execute()
{
    for (auto &plan : m_plans)
    {
        if (m_pProgressDialog && m_pProgressDialog->IsCancelled())
        {
            return *this;
        }
        __Execute(plan.first, plan.second);
    }
    for (auto &spEntry : m_entries)
    {
        if (m_pProgressDialog && m_pProgressDialog->IsCancelled())
        {
            return *this;
        }
        auto spModel = dynamic_pointer_cast<VirtualModel>(
            spEntry->GetModel());
        if (!spModel)
        {
            if (m_pExtractIndicator)
            {
                m_pExtractIndicator->AddError(wxString::Format(
                    _("%s: The archive is either in unknown format or damaged."),
                    spEntry->GetPath()).ToStdWstring());
            }
            continue;
        }
        __Execute(spModel->GetArchive()->GetArchiveEntry(), vector<UINT32>{});
    }
    return *this;
}

void Extractor::__Execute(shared_ptr<SevenZipCore::ArchiveEntry> spArchiveEntry,
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
