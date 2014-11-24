#include "stdwx.h"
#include "ArchiveProperty.h"

#include <future>

#include "PasswordDialog.h"

using namespace std;

boost::optional<TString> ArchiveProperty::GetPassword() const
{
    if (!m_otstrPassword)
    {
        PasswordDialog passwordDialog {
            wxTheApp->GetTopWindow(), wxID_ANY, _("Enter password") };
        if (passwordDialog.ShowModal() == wxID_OK)
        {
            m_otstrPassword = passwordDialog.GetValue();
        }
    }
    return m_otstrPassword;
}
