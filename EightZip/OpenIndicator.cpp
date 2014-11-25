#include "stdwx.h"
#include "OpenIndicator.h"

#include <future>

#include "ArchiveProperty.h"
#include "PasswordDialog.h"
#include "ProgressDialog.h"
#include "ScopeGuard.h"

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
    if (wxThread::IsMain())
    {
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
    }
    else
    {
        promise<boost::optional<TString>> result;
        wxTheApp->CallAfter([&]() {
            if (m_pProgressDialog)
            {
                m_pProgressDialog->Pause();
            }
            result.set_value(m_pArchiveProperty->GetPassword());
        });
        ON_SCOPE_EXIT([&] {
            if (auto *pProgressDialog = m_pProgressDialog)
            {
                wxTheApp->CallAfter([pProgressDialog]() {
                    pProgressDialog->Resume();
                });
            }
        });
        return result.get_future().get();
    }
}
