#include "stdwx.h"
#include "ExtractDialog.h"

#include "EightFilePicker.h"

ExtractDialog::ExtractDialog(
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

void ExtractDialog::__Create()
{
    auto *pSizerMain = new wxBoxSizer(wxVERTICAL);

    pSizerMain->Add(new wxStaticText(
        this,
        wxID_ANY,
        _("Destination")),
        wxSizerFlags().Border(wxTOP | wxLEFT | wxRIGHT, 10));

    pSizerMain->Add(new EightDirPickerCtrl(this, wxID_ANY),
        wxSizerFlags().Border(wxLEFT | wxRIGHT, 10).Expand());

    SetSizer(pSizerMain);
}
