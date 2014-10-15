// OverwriteDialog.h

#ifndef OVERWRITEDIALOG_H
#define OVERWRITEDIALOG_H

#include "SevenZipCore/TString.h"

class OverwriteDialog
    : public wxDialog
{
public:
    OverwriteDialog(
        wxWindow *parent,
        wxWindowID id,
        const wxString& title,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_DIALOG_STYLE,
        const wxString& name = wxDialogNameStr);

    virtual ~OverwriteDialog();

    void SetFilePath(TString tstrPath);

private:
    void __Create();

    wxTextCtrl *m_pTextCtrlPath = nullptr;

};

#endif // OVERWRITEDIALOG_H
