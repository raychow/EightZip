// PasswordDialog.h

#ifndef PASSWORDDIALOG_H
#define PASSWORDDIALOG_H

#include "SevenZipCore/TString.h"

class PasswordDialog
    : public wxDialog
{
public:
    PasswordDialog(
        wxWindow *parent,
        wxWindowID id,
        const wxString& title,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_DIALOG_STYLE,
        const wxString& name = wxDialogNameStr);

    virtual ~PasswordDialog();

    inline TString GetValue() const
    {
        return m_pCheckBoxShowPassword->IsChecked()
            ? m_pTextCtrlPlain->GetValue().ToStdWstring()
            : m_pTextCtrlPassword->GetValue().ToStdWstring();
    }

private:
    wxTextCtrl *m_pTextCtrlPassword = nullptr;
    wxTextCtrl *m_pTextCtrlPlain = nullptr;
    wxCheckBox *m_pCheckBoxShowPassword = nullptr;

    PasswordDialog(const PasswordDialog &) = delete;
    PasswordDialog &operator=(const PasswordDialog &) = delete;

    void __Create();

};

#endif // PASSWORDDIALOG_H
