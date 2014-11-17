// RenameDialog.h

#ifndef RENAMEDIALOG_H
#define RENAMEDIALOG_H

#include "SevenZipCore/TString.h"

class RenameDialog
    : public wxDialog
{
public:
    RenameDialog(
        wxWindow *parent,
        wxWindowID id,
        const wxString& title,
        TString tstrPath,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_DIALOG_STYLE,
        const wxString& name = wxDialogNameStr);

    virtual ~RenameDialog();

    inline TString GetValue() const
    {
        return m_pFilePickerCtrl->GetPath().ToStdWstring();
    }

private:
    wxFilePickerCtrl *m_pFilePickerCtrl = nullptr;

    void __Create(TString tstrPath);

};

#endif // RENAMEDIALOG_H
