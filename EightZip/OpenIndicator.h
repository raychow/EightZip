// OpenIndicator.h

#ifndef OPENINDICATOR_H
#define OPENINDICATOR_H

#include "SevenZipCore/IOpenIndicator.h"

class ProgressDialog;

class OpenIndicator
    : public SevenZipCore::IOpenIndicator
{
public:
    OpenIndicator(ProgressDialog *pProgressDialog)
        : m_pProgressDialog(pProgressDialog)
    { }
    virtual ~OpenIndicator() {}

    virtual void SetTotal(UINT64 un64FileCount, UINT64 un64Size);
    virtual void SetCompleted(UINT64 un64FileCount, UINT64 un64Size);
    virtual boost::optional<TString> GetPassword();

private:
    ProgressDialog *m_pProgressDialog = nullptr;

};

#endif // OPENINDICATOR_H
