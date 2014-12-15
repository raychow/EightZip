// MessageDialog.h

#ifndef MESSAGEDIALOG_H
#define MESSAGEDIALOG_H

#include "SevenZipCore/TString.h"

class MessageDialog
    : public wxDialog
{
public:
    MessageDialog(
        wxWindow *parent,
        wxWindowID id,
        const wxString &title,
        const wxPoint &pos = wxDefaultPosition,
        const wxSize &size = wxDefaultSize,
        long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER,
        const wxString &name = wxDialogNameStr);

    virtual ~MessageDialog();

    void AddMessage(const TString &tstrMessage);

    static void ShowAndAddMessage(const TString &tstrMessage);

private:
    wxListView *m_pListViewMessage = nullptr;

    MessageDialog(const MessageDialog &) = delete;
    MessageDialog &operator=(const MessageDialog &) = delete;

    void __Create();

};

#endif // MESSAGEDIALOG_H
