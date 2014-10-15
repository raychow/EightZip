#include "stdwx.h"
#include "ExtractIndicator.h"

#include <functional>
#include <future>

#include "SevenZipCore/CommonHelper.h"

#include "ProgressDialog.h"

using namespace std;

ExtractIndicator::ExtractIndicator(ProgressDialog *pProgressDialog)
    : m_pProcessDialog(pProgressDialog)
{
}

void ExtractIndicator::SetTotal(UINT64 un64Total)
{
    m_pProcessDialog->SetTotal(un64Total);
}

void ExtractIndicator::SetCompleted(boost::optional<UINT64> oun64Value)
{
    m_pProcessDialog->SetCompleted(oun64Value ? *oun64Value : 0);
}

SevenZipCore::OverwriteAnswer ExtractIndicator::AskOverwrite(
    TString tstrPath,
    boost::optional<time_t> oftExistModified,
    boost::optional<UINT64> un64ExistSize,
    boost::optional<time_t> oftNewModified,
    boost::optional<UINT64> un64NewSize)
{
    promise<SevenZipCore::OverwriteAnswer> result;
    wxTheApp->GetTopWindow()->CallAfter([&](){
        result.set_value(m_pProcessDialog->AskOverwrite(move(tstrPath),
            oftExistModified, un64ExistSize, oftNewModified, un64NewSize));
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
    m_pProcessDialog->SetCurrentFile(
        SevenZipCore::Helper::GetFileName(tstrPath));
}

void ExtractIndicator::SetOperationResult(
    SevenZipCore::ExtractResult extractResult)
{
    //wxMessageBox(wxString::Format("SetOperationResult: %d", extractResult));
}
