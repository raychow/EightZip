// ExtractDialog.h

#ifndef EXTRACTDIALOG_H
#define EXTRACTDIALOG_H

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
    wxComboBox *m_pComboBoxPath = nullptr;

    void __Create();

    void __OnBrowseClick(wxCommandEvent &WXUNUSED(event));

};

#endif // EXTRACTDIALOG_H
