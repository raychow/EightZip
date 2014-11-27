#include "stdwx.h"
#include "ArchiveProperty.h"

#include <future>

#include "PasswordDialog.h"

using namespace std;

boost::optional<TString> ArchiveProperty::GetPassword() const
{
    if (!m_otstrPassword)
    {
        if (wxThread::IsMain())
        {
            PasswordDialog passwordDialog {
                wxTheApp->GetTopWindow(), wxID_ANY, _("Enter password") };
            if (passwordDialog.ShowModal() == wxID_OK)
            {
                m_otstrPassword = passwordDialog.GetValue();
            }
        }
        else
        {
            promise<boost::optional<TString>> result;
            wxTheApp->CallAfter([&]() {
                PasswordDialog passwordDialog {
                    wxTheApp->GetTopWindow(), wxID_ANY, _("Enter password") };
                if (passwordDialog.ShowModal() == wxID_OK)
                {
                    result.set_value(passwordDialog.GetValue());
                }
            });
            m_otstrPassword = result.get_future().get();
        }
    }
    return m_otstrPassword;
}
