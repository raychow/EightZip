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
    m_tstrPath = tstrPath;
}

void OverwriteDialog::__Create()
{
    auto *pSizerMain = new wxBoxSizer(wxVERTICAL);

    auto *pSizerFileInfo = new wxStaticBoxSizer(
        new wxStaticBox(this, wxID_ANY, wxEmptyString), wxVERTICAL);

    pSizerFileInfo->Add(new wxStaticText(this, wxID_ANY,
        _("The following file already exists:")));
    auto pTextCtrlPath = new wxTextCtrl(this, wxID_ANY, m_tstrPath);
    pTextCtrlPath->Enable(false);
    pSizerFileInfo->Add(pTextCtrlPath);

    pSizerMain->Add(pSizerFileInfo);

}
