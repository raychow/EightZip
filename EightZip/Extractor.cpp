#include "stdwx.h"
#include "Extractor.h"

#include <cassert>
#include <queue>

#include "SevenZipCore/ArchiveEntry.h"
#include "SevenZipCore/ArchiveExtractCallback.h"
#include "SevenZipCore/ArchiveFile.h"
#include "SevenZipCore/ComPtr.h"
#include "SevenZipCore/Exception.h"
#include "SevenZipCore/OpenCallback.h"

#include "EntryBase.h"
#include "ExtractIndicator.h"
#include "FileHelper.h"
#include "OpenIndicator.h"
#include "ProgressDialog.h"
#include "VirtualEntry.h"
#include "VirtualModel.h"
#include "VirtualRootModel.h"

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

pair<bool, TString> Extractor::_Execute(
    const SevenZipCore::ArchiveEntry &archiveEntry,
    ArchiveProperty *pArchiveProperty,
    const std::vector<UINT32> &vun32ArchiveIndex)
{
    auto &archive = archiveEntry.GetArchive();
    auto extractIndicator = ExtractIndicator {
        pArchiveProperty, m_pProgressDialog };
//     try
//     {
    auto inArchiveAdapter = SevenZipCore::IInArchiveAdapter<>(
        archiveEntry.GetInArchive());
    auto cpCallback = CreateCallback(
        archive,
        m_tstrExtractLocation,
        m_tstrInternalLocation,
        &extractIndicator);
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
    return { cpCallback->HasError(), cpCallback->GetLastExtractPath() };
//     }
//     catch (const SevenZipCore::ArchiveException &ex)
//     {
// 
//         //if (m_pExtractIndicator
//         //    && m_pProgressDialog && !m_pProgressDialog->IsCancelled())
//         //{
//         //    m_pExtractIndicator->AddError(wxString::Format(
//         //        _("Cannot extract \"%s\"."),
//         //        archive.GetPath()).ToStdWstring());
//         //}
//     }
}

Extractor::ExecutionResult RealFileExtractor::Execute()
{
    auto allSize = wxULongLong_t{};
    auto sizes = vector < UINT64 > {};
    auto vtstrExistPath = vector < TString > {};
    for (const auto &tstrPath : m_vtstrPath)
    {
        try
        {
            sizes.push_back(FileInfo { tstrPath }.GetSize());
            allSize += sizes.back();
            vtstrExistPath.push_back(tstrPath);
        }
        catch (const SevenZipCore::SevenZipCoreException &)
        {
        }
    }
    if (GetProgressDialog())
    {
        GetProgressDialog()->SetAllTotal(allSize);
    }
    auto result = ExecutionResult {};
    for (auto i = 0; i != vtstrExistPath.size(); ++i)
    {
        const auto &tstrPath = vtstrExistPath[i];
        if (GetProgressDialog())
        {
            if (GetProgressDialog()->IsCancelled())
            {
                break;
            }
            GetProgressDialog()->SetArchivePath(tstrPath);
            GetProgressDialog()->SetCurrentTotal(sizes[i]);
        }
        bool isSuccess = false;
        try
        {
            auto spModel = make_shared<VirtualRootModel>(
                Helper::GetLocation(tstrPath),
                SevenZipCore::Helper::GetFileName(tstrPath),
                tstrPath,
                nullptr,
                nullptr);
            auto thisResult = _Execute(spModel->GetArchive().GetArchiveEntry(),
                &spModel->GetProperty(),
                {});
            if (thisResult.first)
            {
                result.first = true;
            }
            result.second = thisResult.second;
            isSuccess = true;
        }
        catch (const SevenZipCore::SevenZipCoreException &)
        {
        }
        if (!isSuccess)
        {
            //if (GetExtractIndicator())
            //{
            //    GetExtractIndicator()->AddError(wxString::Format(
            //        _("%s: The archive is either in unknown format or damaged."),
            //        tstrPath).ToStdWstring());
            //}
        }
    }
    return result;
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

Extractor::ExecutionResult VirtualFileExtractor::Execute()
{
    sort(m_vun32Index.begin(), m_vun32Index.end());
    m_vun32Index.erase(unique(m_vun32Index.begin(), m_vun32Index.end()),
        m_vun32Index.end());
    if (GetProgressDialog())
    {
        GetProgressDialog()->SetArchivePath(m_tstrVirtualArchivePath);
    }
    return _Execute(m_archiveEntry, m_pArchiveProperty, m_vun32Index);
}
