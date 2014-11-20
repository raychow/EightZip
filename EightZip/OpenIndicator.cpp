#include "stdwx.h"
#include "OpenIndicator.h"

#include <future>

#include "PasswordDialog.h"
#include "ProgressDialog.h"

using namespace std;

void OpenIndicator::SetTotal(UINT64 un64FileCount, UINT64 un64Size)
{
}

void OpenIndicator::SetCompleted(UINT64 un64FileCount, UINT64 un64Size)
{
    m_pProgressDialog->CheckCancelled();
}

boost::optional<TString> OpenIndicator::GetPassword()
{
    promise<boost::optional<TString>> result;
    wxTheApp->GetTopWindow()->CallAfter([&]() {
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
    return result.get_future().get();
}
