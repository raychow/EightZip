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
        long style = wxDEFAULT_DIALOG_STYLE,
        const wxString& name = wxDialogNameStr);

    ~ExtractDialog() {}

private:
    void __Create();

};

#endif // EXTRACTDIALOG_H
