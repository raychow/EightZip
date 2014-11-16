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
#include "FileHelper.h"
#include "ProgressDialog.h"
#include "VirtualEntry.h"
#include "VirtualModel.h"

using namespace std;

SevenZipCore::unique_com_ptr<SevenZipCore::ArchiveExtractCallback> CreateCallback(
    SevenZipCore::Archive &archive,
    TString tstrExtractLocation, TString tstrInternalPath,
    SevenZipCore::IExtractIndicator *pExtractIndicator)
{
    return SevenZipCore::MakeUniqueCom(
        new SevenZipCore::ArchiveExtractCallback(archive,
        false,
        false,
        false,
        move(tstrExtractLocation),
        move(tstrInternalPath),
        SevenZipCore::ExtractPathMode::CurrentPathNames,
        SevenZipCore::ExtractOverwriteMode::AskBefore,
        pExtractIndicator));
}

void Extractor::_Execute(const SevenZipCore::ArchiveEntry &archiveEntry,
    const std::vector<UINT32> &vun32ArchiveIndex)
{
    auto &archive = archiveEntry.GetArchive();
    if (m_pProgressDialog)
    {
        m_pProgressDialog->SetArchivePath(archive.GetPath());
    }
    try
    {
        auto inArchiveAdapter = SevenZipCore::IInArchiveAdapter<>(
            archiveEntry.GetInArchive());
        auto cpCallback = CreateCallback(
            archive,
            m_tstrExtractLocation,
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
                archive.GetPath()).ToStdWstring());
        }
    }
}

RealFileExtractor &RealFileExtractor::Execute()
{
    auto allSize = wxULongLong_t{};
    auto vtstrExistPath = vector < TString > {};
    for (const auto &tstrPath : m_vtstrPath)
    {
        try
        {
            allSize += FileInfo{ tstrPath }.GetSize();
            vtstrExistPath.push_back(tstrPath);
        }
        catch (const SevenZipCore::SevenZipCoreException &)
        {
        }
    }
    for (const auto &tstrPath : vtstrExistPath)
    {
        if (GetProgressDialog() && GetProgressDialog()->IsCancelled())
        {
            break;
        }
        bool isSuccess = false;
        try
        {
            auto spModel = make_shared<VirtualModel>(
                Helper::GetLocation(tstrPath),
                SevenZipCore::Helper::GetFileName(tstrPath),
                tstrPath,
                nullptr,
                nullptr);
            _Execute(spModel->GetArchive().GetArchiveEntry(),
                vector < UINT32 > {});
            isSuccess = true;
        }
        catch (const SevenZipCore::SevenZipCoreException &)
        {
        }
        if (!isSuccess)
        {
            if (GetExtractIndicator())
            {
                GetExtractIndicator()->AddError(wxString::Format(
                    _("%s: The archive is either in unknown format or damaged."),
                    tstrPath).ToStdWstring());
            }
        }
    }
    return *this;
}

VirtualFileExtractor &VirtualFileExtractor::AddPlan(const VirtualEntry &entry)
{
    assert(&entry.GetArchiveFile().GetArchiveEntry() == &m_archiveEntry);
    m_vun32Index.push_back(entry.GetArchiveFile().GetIndex());
    if (entry.IsDirectory())
    {
        auto spModel = dynamic_pointer_cast<VirtualModel>(entry.GetModel());
        AddPlan(*spModel);
    }
    return *this;
}

VirtualFileExtractor &VirtualFileExtractor::AddPlan(const VirtualModel &model)
{
    assert(&model.GetArchiveFolder().GetArchiveEntry() == &m_archiveEntry);
    auto vun32ModelIndex = model.GetArchiveFolder().GetAllIndexes();
    m_vun32Index.insert(m_vun32Index.end(),
        vun32ModelIndex.cbegin(),
        vun32ModelIndex.cend());
    return *this;
}

VirtualFileExtractor &VirtualFileExtractor::Execute()
{
    sort(m_vun32Index.begin(), m_vun32Index.end());
    m_vun32Index.erase(unique(m_vun32Index.begin(), m_vun32Index.end()),
        m_vun32Index.end());
    _Execute(m_archiveEntry, m_vun32Index);
    return *this;
}
