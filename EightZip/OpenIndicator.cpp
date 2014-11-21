#include "stdwx.h"
#include "OpenIndicator.h"

#include <future>

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
    promise<boost::optional<TString>> result;
    wxTheApp->CallAfter([&]() {
        if (m_pProgressDialog)
        {
            m_pProgressDialog->Pause();
        }
        PasswordDialog passwordDialog {
            wxTheApp->GetTopWindow(), wxID_ANY, _("Enter password") };
        if (passwordDialog.ShowModal() == wxID_OK)
        {
            result.set_value(passwordDialog.GetValue());
        }
        else
        {
            result.set_value(boost::none);
        }
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
