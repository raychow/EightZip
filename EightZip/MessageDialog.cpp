#include "stdwx.h"
#include "MessageDialog.h"

MessageDialog::MessageDialog(wxWindow *parent,
    wxWindowID id,
    const wxString &title,
    const wxPoint &pos /*= wxDefaultPosition*/,
    const wxSize &size /*= wxDefaultSize*/,
    long style /*= wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER*/,
    const wxString &name /*= wxDialogNameStr*/)
    : wxDialog(parent, id, title, pos, size, style, name)
{
    __Create();
}

MessageDialog::~MessageDialog()
{

}

void MessageDialog::AddMessage(const TString &tstrMessage)
{
    auto itemCount = m_pListViewMessage->GetItemCount();
    auto index = m_pListViewMessage->InsertItem(
        itemCount, ToTString(itemCount + 1));
    if (-1 != index)
    {
        m_pListViewMessage->SetItem(index, 1, tstrMessage);
    }
}

void MessageDialog::ShowAndAddMessage(const TString &tstrMessage)
{
    static auto *pDialog = new MessageDialog {
        wxTheApp->GetTopWindow(), wxID_ANY, _("Messages") };
    pDialog->AddMessage(tstrMessage);
    pDialog->Show();
}

void MessageDialog::__Create()
{
    auto *pSizerMain = new wxBoxSizer(wxVERTICAL);

    m_pListViewMessage = new wxListView { this, wxID_ANY };
    m_pListViewMessage->InsertColumn(0, wxEmptyString, wxLIST_FORMAT_RIGHT);
    m_pListViewMessage->InsertColumn(1, _("Message"), wxLIST_FORMAT_LEFT,
        wxLIST_AUTOSIZE_USEHEADER);
    pSizerMain->Add(m_pListViewMessage,
        wxSizerFlags().Border(wxALL, 7).Expand().Proportion(1));

    auto *pSizerButton = new wxBoxSizer(wxHORIZONTAL);
    pSizerButton->Add(new wxButton { this, wxID_ANY, _("&Close") },
        wxSizerFlags {}.Border(wxRIGHT, 5));
    pSizerButton->Add(new wxButton { this, wxID_ANY, _("&Break operation") },
        wxSizerFlags {}.Border(wxRIGHT, 5));
    pSizerButton->Add(new wxButton { this, wxID_ANY, _("Copy to &clipboard") });
    pSizerMain->Add(pSizerButton,
        wxSizerFlags().Border(wxBOTTOM, 7).Center());

    SetSizer(pSizerMain);
    SetSize(500, 280);
    Layout();
    //CenterOnParent();

    Bind(wxEVT_CLOSE_WINDOW, [&](wxCloseEvent &event) {
        m_pListViewMessage->DeleteAllItems();
        event.Skip();
    });
}
