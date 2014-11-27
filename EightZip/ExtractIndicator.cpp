#include "stdwx.h"
#include "ExtractIndicator.h"

#include <functional>

#include "SevenZipCore/CommonHelper.h"

#include "ArchiveProperty.h"
#include "OverwriteDialog.h"
#include "ProgressDialog.h"
#include "ScopeGuard.h"
#include "ThreadHelper.h"

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

    return Helper::CallOnMainThread([&] {
        if (m_pProcessDialog)
        {
            m_pProcessDialog->Pause();
        }
        ON_SCOPE_EXIT([&] {
            if (m_pProcessDialog)
            {
                if (SevenZipCore::OverwriteAnswer::Cancel
                    == m_lastOverwriteAnswer)
                {
                    m_pProcessDialog->Cancel();
                }
                else
                {
                    m_pProcessDialog->Resume();
                }
            }
        });
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
        return m_lastOverwriteAnswer;
    });
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

boost::optional<TString> ExtractIndicator::GetPassword() const
{
    if (m_pArchiveProperty)
    {
        return m_pArchiveProperty->GetPassword();
    }
    return boost::none;
}
