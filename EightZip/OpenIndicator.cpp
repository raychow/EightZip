#include "stdwx.h"
#include "OpenIndicator.h"

#include "ArchiveProperty.h"
#include "ProgressDialog.h"
#include "ScopeGuard.h"
#include "ThreadHelper.h"

using namespace std;

void OpenIndicator::SetTotal(UINT64 un64FileCount, UINT64 un64Size)
{
}

void OpenIndicator::SetCompleted(UINT64 un64FileCount, UINT64 un64Size)
{
    if (m_pProgressDialog)
    {
        m_pProgressDialog->CheckCancelled();
    }
}

boost::optional<TString> OpenIndicator::GetPassword()
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
        m_pProgressDialog->CancelDelay();
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
