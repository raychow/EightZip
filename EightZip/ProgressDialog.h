// ProgressDialog.h

#ifndef PROGRESSDIALOG_H
#define PROGRESSDIALOG_H

class ProgressDialog
    : public wxDialog
{
public:
    ProgressDialog(
        wxWindow *parent,
        wxWindowID id,
        const wxString& title,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_DIALOG_STYLE,
        const wxString& name = wxDialogNameStr);

    ~ProgressDialog() {}

private:
    wxGauge *m_pGaugeCurrentFile = nullptr;
    wxGauge *m_pGaugeProcessed = nullptr;

    void __Create();

};

#endif // PROGRESSDIALOG_H
