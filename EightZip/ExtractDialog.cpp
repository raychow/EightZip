#include "stdwx.h"
#include "ExtractDialog.h"

#include <wx/filepicker.h>

#include "EightDirDialog.h"
#include "WindowHelper.h"

ExtractDialog::ExtractDialog(
    wxWindow *parent,
    wxWindowID id,
    const wxString &title,
    const wxPoint &pos /*= wxDefaultPosition*/,
    const wxSize &size /*= wxDefaultSize*/,
    long style /*= wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER*/,
    const wxString& name /*= wxDialogNameStr*/)
    : wxDialog(parent, id, title, pos, size, style, name)
    , m_windowStateManager(this,
    ConfigIndex::ExtractLocationX,
    ConfigIndex::ExtractLocationY,
    ConfigIndex::ExtractWidth,
    ConfigIndex::ExtractHeight,
    ConfigIndex::ExtractIsMaximized)
{
    __Create();
}

void ExtractDialog::__Create()
{
    auto *pSizerMain = new wxBoxSizer(wxVERTICAL);

    pSizerMain->Add(new wxStaticText(
        this,
        wxID_ANY,
        _("&Destination:")),
        TOP_LEFT_SIZERFLAGS());

    auto *pSizerPath = new wxBoxSizer(wxHORIZONTAL);
    m_pComboBoxPath = new wxComboBox(this, wxID_ANY);
    m_pComboBoxPath->AutoCompleteDirectories();
    pSizerPath->Add(m_pComboBoxPath, TOP_LEFT_SIZERFLAGS_(1).Center());
    auto *pButtonBrowse = new wxButton(this, wxID_ANY, _("&Browse"));
    pSizerPath->Add(pButtonBrowse, TOP_HORIZON_SIZERFLAGS());
    pSizerMain->Add(pSizerPath, wxSizerFlags().Expand());

    m_pCheckBoxLaunchFolder = new wxCheckBox(this, wxID_ANY,
        _("&Launch the folder after extracting"));
    pSizerMain->Add(m_pCheckBoxLaunchFolder, TOP_LEFT_SIZERFLAGS());

    pSizerMain->AddStretchSpacer();

    auto *pSizerDialogButton = new wxBoxSizer(wxHORIZONTAL);
    auto *pButtonMore = new wxButton(this, wxID_ANY, _T("&More"));
    pSizerDialogButton->Add(pButtonMore, BOTTOM_LEFT_SIZERFLAGS());
    pSizerDialogButton->AddStretchSpacer();
    auto *pStdDialogButtonSizer = CreateStdDialogButtonSizer(wxOK | wxCANCEL);
    pSizerDialogButton->Add(Helper::AlignBorder(
        pStdDialogButtonSizer),
        BOTTOM_SIZERFLAGS());
    m_pButtonOK = pStdDialogButtonSizer->GetAffirmativeButton();
    pSizerMain->Add(pSizerDialogButton, wxSizerFlags().Expand());

    SetSizer(pSizerMain);
    SetMinClientSize(wxSize(300, 150));
    m_windowStateManager.LoadState();

    m_pComboBoxPath->Bind(wxEVT_TEXT, &ExtractDialog::__OnPathChange, this);
    pButtonBrowse->Bind(wxEVT_BUTTON, &ExtractDialog::__OnBrowseClick, this);
    m_pButtonOK->Bind(wxEVT_BUTTON, &ExtractDialog::__OnOKClick, this);
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

void ExtractDialog::__OnOKClick(wxCommandEvent &WXUNUSED(event))
{
    auto strPath = m_pComboBoxPath->GetValue().Trim();
    if (strPath.IsEmpty())
    {
        return;
    }
    EndModal(wxID_OK);
}

void ExtractDialog::__OnPathChange(wxCommandEvent &event)
{
    m_pButtonOK->Enable(!event.GetString().Trim().IsEmpty());
}