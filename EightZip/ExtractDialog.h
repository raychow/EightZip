// ExtractDialog.h

#ifndef EXTRACTDIALOG_H
#define EXTRACTDIALOG_H

#include "SevenZipCore/TString.h"

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

    TString GetPath() const
    {
        return m_pComboBoxPath->GetValue().ToStdWstring();
    }

private:
    WindowStateManager m_windowStateManager;

    wxComboBox *m_pComboBoxPath = nullptr;
    wxCheckBox *m_pCheckBoxLaunchFolder = nullptr;
    wxButton *m_pButtonOK = nullptr;

    void __Create();

    void __OnBrowseClick(wxCommandEvent &WXUNUSED(event));
    void __OnOKClick(wxCommandEvent &WXUNUSED(event));
    void __OnPathChange(wxCommandEvent &event);
};

#endif // EXTRACTDIALOG_H
