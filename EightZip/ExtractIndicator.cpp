#include "stdwx.h"
#include "ExtractIndicator.h"

#include <functional>

#include "SevenZipCore/CommonHelper.h"

#include "ArchiveProperty.h"
#include "Exception.h"
#include "MessageDialog.h"
#include "OverwriteDialog.h"
#include "ProgressDialog.h"
#include "ScopeGuard.h"
#include "ThreadHelper.h"

using namespace std;

void ExtractIndicator::SetTotal(UINT64 un64Total)
{
    if (m_pProgressDialog)
    {
        m_pProgressDialog->SetTotal(un64Total);
    }
}

void ExtractIndicator::SetCompleted(boost::optional<UINT64> oun64Value)
{
    if (m_pProgressDialog)
    {
        m_pProgressDialog->SetCompleted(oun64Value ? *oun64Value : 0);
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

    if (m_pProgressDialog)
    {
        Helper::InvokeOnMainThread([&] {
            m_pProgressDialog->CancelDelay();
        });
    }
    return Helper::CallOnMainThread([&] {
        if (m_pProgressDialog)
        {
            m_pProgressDialog->Pause();
        }
        ON_SCOPE_EXIT([&] {
            if (m_pProgressDialog)
            {
                if (SevenZipCore::OverwriteAnswer::Cancel
                    == m_lastOverwriteAnswer)
                {
                    m_pProgressDialog->Cancel();
                }
                else
                {
                    m_pProgressDialog->Resume();
                }
            }
        });
        OverwriteDialog dialog { wxTheApp->GetTopWindow(),
            wxID_ANY,
            _("Confirm file replace"),
            tstrPath,
            oftExistModified,
            oun64ExistSize,
            oftNewModified,
            oun64NewSize };
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
    Helper::InvokeOnMainThread([&] {
        MessageDialog::ShowAndAddMessage(tstrMessage);
    });
}

void ExtractIndicator::AddError(TString tstrMessage, TString tstrParameter)
{
    Helper::InvokeOnMainThread([&] {
        MessageDialog::ShowAndAddMessage(
            wxString::Format(_(tstrMessage), tstrParameter).ToStdWstring());
    });
}

void ExtractIndicator::Prepare(TString tstrPath,
    bool isFolder,
    SevenZipCore::ExtractAskMode askMode,
    boost::optional<UINT64> oun64Position)
{
    if (m_pProgressDialog)
    {
        m_pProgressDialog->SetCurrentFile(
            SevenZipCore::Helper::GetFileName(tstrPath));
    }
}

void ExtractIndicator::SetOperationResult(
    SevenZipCore::ExtractResult extractResult)
{
    auto tstrMessage = TString {};
    switch (extractResult)
    {
    case SevenZipCore::ExtractResult::OK:
        return;
    case SevenZipCore::ExtractResult::UnSupportedMethod:
        tstrMessage = _("Unsupported compression method.");
        break;
    case SevenZipCore::ExtractResult::DataError:
        tstrMessage = _("Data error.");
        break;
    case SevenZipCore::ExtractResult::CRCError:
        tstrMessage = _("CRC failed.");
        break;
    default:
        throw ArchiveException("Not supported extract result.");
    }
    Helper::InvokeOnMainThread([&] {
        MessageDialog::ShowAndAddMessage(tstrMessage);
    });
}

boost::optional<TString> ExtractIndicator::GetPassword() const
{
    if (!m_pArchiveProperty)
    {
        return boost::none;
    }
    if (m_pArchiveProperty->IsSetPassword())
    {
        return m_pArchiveProperty->GetPassword();
    }
    if (m_pProgressDialog)
    {
        Helper::InvokeOnMainThread([&] {
            m_pProgressDialog->CancelDelay();
        });
    }
    return Helper::CallOnMainThread([&] {
        if (m_pProgressDialog)
        {
            m_pProgressDialog->Pause();
        }
        ON_SCOPE_EXIT([&] {
            if (m_pProgressDialog)
            {
                m_pProgressDialog->Resume();
            }
        });
        return m_pArchiveProperty->GetPassword();
    });
}
