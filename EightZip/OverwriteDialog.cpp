#include "stdwx.h"
#include "OverwriteDialog.h"


OverwriteDialog::OverwriteDialog(
    wxWindow *parent,
    wxWindowID id,
    const wxString& title,
    const wxPoint& pos /*= wxDefaultPosition*/,
    const wxSize& size /*= wxDefaultSize*/,
    long style /*= wxDEFAULT_DIALOG_STYLE*/,
    const wxString& name /*= wxDialogNameStr*/)
    : wxDialog(parent, id, title, pos, size, style, name)
{
    __Create();
}

OverwriteDialog::~OverwriteDialog()
{

}

void OverwriteDialog::SetFilePath(TString tstrPath)
{
    m_pTextCtrlPath->SetValue(tstrPath);
}

void OverwriteDialog::__Create()
{
    auto *pSizerMain = new wxBoxSizer(wxVERTICAL);

    auto *pSizerFileInfo = new wxStaticBoxSizer(
        new wxStaticBox(this, wxID_ANY, wxEmptyString), wxVERTICAL);

    pSizerFileInfo->Add(new wxStaticText(this, wxID_ANY,
        _("The following file already exists:")));
    m_pTextCtrlPath = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
        wxDefaultPosition, wxSize(-1, 70), wxTE_MULTILINE);
    m_pTextCtrlPath->SetEditable(false);
    pSizerFileInfo->Add(m_pTextCtrlPath, wxSizerFlags().Expand());

    pSizerMain->Add(pSizerFileInfo,
        wxSizerFlags().Expand().Border(wxLEFT | wxRIGHT, 8));

    SetSizer(pSizerMain);
}
