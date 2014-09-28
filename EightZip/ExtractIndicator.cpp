#include "stdwx.h"
#include "ExtractIndicator.h"

#include <functional>

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
    TString tstrExistPath,
    boost::optional<FILETIME> oftExistModified,
    boost::optional<UINT64> un64ExistSize,
    TString tstrNewPath,
    boost::optional<FILETIME> oftNewModified,
    boost::optional<UINT64> un64NewSize)
{
    return SevenZipCore::OverwriteAnswer::Yes;
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
