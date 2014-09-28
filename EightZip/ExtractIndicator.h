// ExtractIndicator.h

#ifndef EXTRACTINDICATOR_H
#define EXTRACTINDICATOR_H

#include <memory>

#include "SevenZipCore/IExtractIndicator.h"
#include "SevenZipCore/TString.h"

class ProgressDialog;

class ExtractIndicator
    : public SevenZipCore::IExtractIndicator
{
public:
    ExtractIndicator(ProgressDialog *pProgressDialog);

    virtual void SetTotal(UINT64 un64Total);
    virtual void SetCompleted(boost::optional<UINT64> oun64Value);
    virtual SevenZipCore::OverwriteAnswer AskOverwrite(TString tstrExistPath,
        boost::optional<FILETIME> oftExistModified,
        boost::optional<UINT64> un64ExistSize,
        TString tstrNewPath,
        boost::optional<FILETIME> oftNewModified,
        boost::optional<UINT64> un64NewSize);
    virtual void AddError(TString tstrMessage);
    virtual void AddError(TString tstrMessage, TString tstrParameter);
    virtual void Prepare(TString tstrPath,
        bool isFolder,
        SevenZipCore::ExtractAskMode askMode,
        boost::optional<UINT64> oun64Position);
    virtual void SetOperationResult(
        SevenZipCore::ExtractResult extractResult);

private:
    ProgressDialog *m_pProcessDialog;

};

#endif // EXTRACTINDICATOR_H
