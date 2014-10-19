#include "stdwx.h"
#include "RenameDialog.h"

RenameDialog::RenameDialog(
    wxWindow *parent,
    wxWindowID id,
    const wxString& title,
    TString tstrPath,
    const wxPoint& pos /*= wxDefaultPosition*/,
    const wxSize& size /*= wxDefaultSize*/,
    long style /*= wxDEFAULT_DIALOG_STYLE*/,
    const wxString& name /*= wxDialogNameStr*/)
    : wxDialog(parent, id, title, pos, size, style, name)
{
    __Create(tstrPath);
}

RenameDialog::~RenameDialog()
{

}

void RenameDialog::__Create(TString tstrPath)
{
    auto *pSizerMain = new wxBoxSizer(wxVERTICAL);

    auto *pSizerPath = new wxBoxSizer(wxVERTICAL);

    pSizerPath->Add(new wxStaticText(this, wxID_ANY, _("Rename file")));
    auto *pTextCtrlExistFile = new wxTextCtrl(this, wxID_ANY, tstrPath);
    pTextCtrlExistFile->SetEditable(false);
    pSizerPath->Add(pTextCtrlExistFile, wxSizerFlags().Expand());

    pSizerPath->Add(new wxStaticText(this, wxID_ANY, _("to")),
        wxSizerFlags().Border(wxTOP, 3));
    m_pTextCtrlNewPath = new wxTextCtrl(this, wxID_ANY, tstrPath);
    pSizerPath->Add(m_pTextCtrlNewPath, wxSizerFlags().Expand());

    pSizerMain->Add(pSizerPath,
        wxSizerFlags().Border(wxTOP | wxRIGHT | wxLEFT, 6).Expand());

    pSizerMain->Add(CreateStdDialogButtonSizer(wxOK | wxCANCEL),
        wxSizerFlags().Border(wxTOP, 12).Center());

    SetSizer(pSizerMain);
}
