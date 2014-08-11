#include "stdwx.h"
#include "EightFilePicker.h"

#ifdef __WXMSW__

#include "EightDirDialog.h"

IMPLEMENT_DYNAMIC_CLASS(EightGenericDirButton, wxGenericDirButton)
IMPLEMENT_DYNAMIC_CLASS(EightDirPickerCtrl, wxDirPickerCtrl)

wxDialog *EightGenericDirButton::CreateDialog()
{
    wxDirDialog* const dialog = new EightDirDialog
        (
        GetDialogParent(),
        m_message,
        m_path.empty() ? m_initialDir : m_path,
        GetDialogStyle()
        );
    return dialog;
}

#endif