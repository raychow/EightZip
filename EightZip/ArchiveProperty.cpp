#include "stdwx.h"
#include "ArchiveProperty.h"

#include "PasswordDialog.h"
#include "ThreadHelper.h"

using namespace std;

boost::optional<TString> ArchiveProperty::GetPassword() const
{
    if (!m_otstrPassword)
    {
        Helper::InvokeOnMainThread([&] {
            PasswordDialog passwordDialog {
                wxTheApp->GetTopWindow(), wxID_ANY, _("Enter password") };
            if (passwordDialog.ShowModal() == wxID_OK)
            {
                m_otstrPassword = passwordDialog.GetValue();
            }
        });
    }
    return m_otstrPassword;
}
