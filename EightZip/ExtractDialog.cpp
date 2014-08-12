#include "stdwx.h"
#include "ExtractDialog.h"

#include "EightDirDialog.h"
#include "Helper.h"

ExtractDialog::ExtractDialog(
    wxWindow *parent,
    wxWindowID id,
    const wxString& title,
    const wxPoint& pos /*= wxDefaultPosition*/,
    const wxSize& size /*= wxDefaultSize*/,
    long style /*= wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER*/,
    const wxString& name /*= wxDialogNameStr*/)
    : wxDialog(parent, id, title, pos, size, style, name)
{
    __Create();
}

void ExtractDialog::__Create()
{
    auto *pSizerMain = new wxBoxSizer(wxVERTICAL);

    pSizerMain->Add(new wxStaticText(
        this,
        wxID_ANY,
        _("Destination:")),
        TOP_LEFT_SIZERFLAGS());

    // Path
    auto *pSizerPath = new wxBoxSizer(wxHORIZONTAL);
    
    m_pComboBoxPath = new wxComboBox(this, wxID_ANY);
    m_pComboBoxPath->AutoCompleteDirectories();
    pSizerPath->Add(m_pComboBoxPath, TOP_LEFT_SIZERFLAGS_(1).Center());

    auto *pButtonBrowse = new wxButton(this, wxID_ANY, _("Browse"));
    pButtonBrowse->Bind(wxEVT_BUTTON, &ExtractDialog::__OnBrowseClick, this);
    pSizerPath->Add(pButtonBrowse, TOP_HORIZON_SIZERFLAGS());

    pSizerMain->Add(pSizerPath, wxSizerFlags().Expand());
    pSizerMain->AddStretchSpacer(1);

    pSizerMain->Add(Helper::AlignBorder(
        CreateStdDialogButtonSizer(wxOK | wxCLOSE)),
        BOTTOM_SIZERFLAGS().Expand());

    SetSizer(pSizerMain);
}

void ExtractDialog::__OnBrowseClick(wxCommandEvent &WXUNUSED(event))
{
    EightDirDialog dialog(this,
        _("Select a directory"),
        m_pComboBoxPath->GetValue());
    if (dialog.ShowModal() == wxID_OK)
    {
        m_pComboBoxPath->SetValue(dialog.GetPath());
    }
}
