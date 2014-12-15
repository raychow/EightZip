#include "stdwx.h"
#include "PasswordDialog.h"

PasswordDialog::PasswordDialog(
    wxWindow *parent,
    wxWindowID id,
    const wxString &title,
    const wxPoint &pos /*= wxDefaultPosition*/,
    const wxSize &size /*= wxDefaultSize*/,
    long style /*= wxDEFAULT_DIALOG_STYLE*/,
    const wxString &name /*= wxDialogNameStr*/)
    : wxDialog(parent, id, title, pos, size, style, name)
{
    __Create();
}

PasswordDialog::~PasswordDialog()
{

}

void PasswordDialog::__Create()
{
    auto *pSizerMain = new wxBoxSizer(wxVERTICAL);

    auto *pSizerPassword = new wxBoxSizer(wxVERTICAL);

    pSizerPassword->Add(new wxStaticText(this, wxID_ANY, _("&Enter password:")));
    auto *pSizerTextCtrl = new wxBoxSizer(wxVERTICAL);
    m_pTextCtrlPassword = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
        wxDefaultPosition, wxSize(200, -1), wxTE_PASSWORD);
    pSizerTextCtrl->Add(m_pTextCtrlPassword);
    pSizerPassword->Add(pSizerTextCtrl,
        wxSizerFlags().Expand().Border(wxBOTTOM, 6));
    m_pCheckBoxShowPassword = new wxCheckBox(
        this, wxID_ANY, _("&Show password"));
    pSizerPassword->Add(m_pCheckBoxShowPassword);

    pSizerMain->Add(pSizerPassword,
        wxSizerFlags().Border(wxTOP | wxRIGHT | wxLEFT, 12).Expand());

    auto pSizerStdDialogButton = CreateStdDialogButtonSizer(wxOK | wxCANCEL);
    auto pButtonOK = pSizerStdDialogButton->GetAffirmativeButton();
    pButtonOK->Enable(false);
    pSizerMain->AddSpacer(6);
    pSizerMain->Add(pSizerStdDialogButton,
        wxSizerFlags().Border(wxTOP | wxBOTTOM, 6).Center());

    m_pTextCtrlPassword->SetFocus();

    SetSizerAndFit(pSizerMain);
    CenterOnParent();

    m_pTextCtrlPassword->Bind(wxEVT_TEXT,
        [this, pButtonOK](wxCommandEvent &WXUNUSED(event)) {
        pButtonOK->Enable(!m_pTextCtrlPassword->GetValue().empty());
    });
    m_pCheckBoxShowPassword->Bind(wxEVT_CHECKBOX,
        [=](wxCommandEvent &event) {
        if (!m_pTextCtrlPlain)
        {
            m_pTextCtrlPlain = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
                wxDefaultPosition, wxSize(200, -1));
            m_pTextCtrlPlain->Bind(wxEVT_TEXT,
                [this, pButtonOK](wxCommandEvent &WXUNUSED(event)) {
                pButtonOK->Enable(!m_pTextCtrlPlain->GetValue().empty());
            });
        }
        pSizerTextCtrl->Clear();
        if (event.IsChecked())
        {
            m_pTextCtrlPlain->SetValue(m_pTextCtrlPassword->GetValue());
            m_pTextCtrlPassword->Hide();
            m_pTextCtrlPlain->Show();
            pSizerTextCtrl->Add(m_pTextCtrlPlain);
        }
        else
        {
            m_pTextCtrlPassword->SetValue(m_pTextCtrlPlain->GetValue());
            m_pTextCtrlPassword->Show();
            m_pTextCtrlPlain->Hide();
            pSizerTextCtrl->Add(m_pTextCtrlPassword);
        }
        pSizerMain->Layout();
    });
}
