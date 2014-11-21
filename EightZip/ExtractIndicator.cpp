#include "stdwx.h"
#include "ExtractIndicator.h"

#include <functional>
#include <future>

#include "SevenZipCore/CommonHelper.h"

#include "OverwriteDialog.h"
#include "ProgressDialog.h"
#include "ScopeGuard.h"

using namespace std;

void ExtractIndicator::SetTotal(UINT64 un64Total)
{
    if (m_pProcessDialog)
    {
        m_pProcessDialog->SetTotal(un64Total);
    }
}

void ExtractIndicator::SetCompleted(boost::optional<UINT64> oun64Value)
{
    if (m_pProcessDialog)
    {
        m_pProcessDialog->SetCompleted(oun64Value ? *oun64Value : 0);
    }
}

SevenZipCore::OverwriteAnswer ExtractIndicator::AskOverwrite(
    TString tstrPath,
    boost::optional<time_t> oftExistModified,
    boost::optional<UINT64> oun64ExistSize,
    boost::optional<time_t> oftNewModified,
    boost::optional<UINT64> oun64NewSize,
    TString *ptstrNewPath)
{
    switch (m_lastOverwriteAnswer)
    {
    case SevenZipCore::OverwriteAnswer::YesToAll:
    case SevenZipCore::OverwriteAnswer::NoToAll:
    case SevenZipCore::OverwriteAnswer::AutoRename:
    case SevenZipCore::OverwriteAnswer::Cancel:
        return m_lastOverwriteAnswer;
    }
    promise<SevenZipCore::OverwriteAnswer> result;
    wxTheApp->CallAfter([&](){
        if (m_pProcessDialog)
        {
            m_pProcessDialog->Pause();
        }
        OverwriteDialog dialog(wxTheApp->GetTopWindow(),
            wxID_ANY,
            _("Confirm file replace"),
            tstrPath,
            oftExistModified,
            oun64ExistSize,
            oftNewModified,
            oun64NewSize);
        m_lastOverwriteAnswer = static_cast<SevenZipCore::OverwriteAnswer>(
            dialog.ShowModal());
        if (ptstrNewPath
            && SevenZipCore::OverwriteAnswer::Rename == m_lastOverwriteAnswer)
        {
            *ptstrNewPath = dialog.GetPath();
        }
        result.set_value(m_lastOverwriteAnswer);
    });
    ON_SCOPE_EXIT([&] {
        if (auto *pProgressDialog = m_pProcessDialog)
        {
            auto lastOverwriteAnswer = m_lastOverwriteAnswer;
            wxTheApp->CallAfter([lastOverwriteAnswer, pProgressDialog]() {
                if (SevenZipCore::OverwriteAnswer::Cancel
                    == lastOverwriteAnswer)
                {
                    pProgressDialog->Cancel();
                }
                else
                {
                    pProgressDialog->Resume();
                }
            });
        }
    });
    return result.get_future().get();
}

void ExtractIndicator::AddError(TString tstrMessage)
{
    //wxMessageBox(wxString::Format("AddError: %s", tstrMessage));
}

void ExtractIndicator::AddError(TString tstrMessage, TString tstrParameter)
{
    //wxMessageBox(wxString::Format("AddError: %s %s", tstrMessage, tstrParameter));
}

void ExtractIndicator::Prepare(TString tstrPath,
    bool isFolder,
    SevenZipCore::ExtractAskMode askMode,
    boost::optional<UINT64> oun64Position)
{
    if (m_pProcessDialog)
    {
        m_pProcessDialog->SetCurrentFile(
            SevenZipCore::Helper::GetFileName(tstrPath));
    }
}

void ExtractIndicator::SetOperationResult(
    SevenZipCore::ExtractResult extractResult)
{
    //wxMessageBox(wxString::Format("SetOperationResult: %d", extractResult));
}
