// ExtractDialog.h

#ifndef EXTRACTDIALOG_H
#define EXTRACTDIALOG_H

#include "WindowStateManager.h"

class ExtractDialog
    : public wxDialog
{
public:
    ExtractDialog(
        wxWindow *parent,
        wxWindowID id,
        const wxString& title,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER,
        const wxString& name = wxDialogNameStr);

    ~ExtractDialog() {}

private:
    WindowStateManager m_windowStateManager;

    wxComboBox *m_pComboBoxPath = nullptr;
    wxCheckBox *m_pCheckBoxLaunchFolder = nullptr;

    void __Create();

    void __OnBrowseClick(wxCommandEvent &WXUNUSED(event));

};

#endif // EXTRACTDIALOG_H
