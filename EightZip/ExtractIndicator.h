// ExtractIndicator.h

#ifndef EXTRACTINDICATOR_H
#define EXTRACTINDICATOR_H

#include <memory>

#include "SevenZipCore/IArchive.h"
#include "SevenZipCore/IExtractIndicator.h"
#include "SevenZipCore/TString.h"

class ArchiveProperty;
class ProgressDialog;

class ExtractIndicator
    : public SevenZipCore::IExtractIndicator
{
public:
    ExtractIndicator(ArchiveProperty *pArchiveProperty,
        ProgressDialog *pProgressDialog)
        : m_pArchiveProperty(pArchiveProperty)
        , m_pProgressDialog(pProgressDialog)
    { }

    virtual void SetTotal(UINT64 un64Total) override;
    virtual void SetCompleted(boost::optional<UINT64> oun64Value) override;
    virtual SevenZipCore::OverwriteAnswer AskOverwrite(TString tstrPath,
        boost::optional<time_t> oftExistModified,
        boost::optional<UINT64> oun64ExistSize,
        boost::optional<time_t> oftNewModified,
        boost::optional<UINT64> oun64NewSize,
        TString *ptstrNewPath) override;
    virtual void AddError(TString tstrMessage) override;
    virtual void AddError(TString tstrMessage, TString tstrParameter) override;
    virtual void Prepare(TString tstrPath,
        bool isFolder,
        SevenZipCore::ExtractAskMode askMode,
        boost::optional<UINT64> oun64Position) override;
    virtual void SetOperationResult(
        SevenZipCore::ExtractResult extractResult, bool isEncrypted) override;
    virtual boost::optional<TString> GetPassword() const override;

    inline ProgressDialog *GetProgressDialog() const
    {
        return m_pProgressDialog;
    }

private:
    ArchiveProperty *m_pArchiveProperty = nullptr;
    ProgressDialog *m_pProgressDialog = nullptr;

    TString m_tstrVirtualPath;

    SevenZipCore::OverwriteAnswer m_lastOverwriteAnswer
        = SevenZipCore::OverwriteAnswer::Unknown;

};

#endif // EXTRACTINDICATOR_H
