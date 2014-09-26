#include "stdwx.h"
#include "ExtractIndicator.h"

#include <functional>

#include "ProgressDialog.h"

using namespace std;

ExtractIndicator::ExtractIndicator(shared_ptr<ProgressDialog> spProgressDialog)
    : m_spProcessDialog(spProgressDialog)
{
}

void ExtractIndicator::SetTotal(UINT64 un64Total)
{
}

void ExtractIndicator::SetCompleted(boost::optional<UINT64> oun64Value)
{
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
}

void ExtractIndicator::AddError(TString tstrMessage, TString tstrParameter)
{

}

void ExtractIndicator::Prepare(TString tstrPath,
    bool isFolder,
    SevenZipCore::ExtractAskMode askMode,
    boost::optional<UINT64> oun64Position)
{
}

void ExtractIndicator::SetOperationResult(
    SevenZipCore::ExtractResult extractResult)
{

}
