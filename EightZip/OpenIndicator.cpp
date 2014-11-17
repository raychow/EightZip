#include "stdwx.h"
#include "OpenIndicator.h"

#include "PasswordDialog.h"

void OpenIndicator::SetTotal(UINT64 un64FileCount, UINT64 un64Size)
{
}

void OpenIndicator::SetCompleted(UINT64 un64FileCount, UINT64 un64Size)
{
}

boost::optional<TString> OpenIndicator::GetPassword()
{
    auto passwordDialog = PasswordDialog {
        wxTheApp->GetTopWindow(), wxID_ANY, _("Enter password") };
    if (passwordDialog.ShowModal() == wxID_OK)
    {
        return passwordDialog.GetValue();
    }
    return boost::none;
}
