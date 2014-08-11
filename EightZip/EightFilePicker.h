// EightFilePicker.h

#ifndef EIGHTFILEPICKER_H
#define EIGHTFILEPICKER_H

#include <wx/filepicker.h>

#ifdef __WXMSW__

class EightGenericDirButton
    : public wxGenericDirButton
{
public:
    EightGenericDirButton() {}
    EightGenericDirButton(wxWindow *parent,
        wxWindowID id,
        const wxString& label = wxDirPickerWidgetLabel,
        const wxString& path = wxEmptyString,
        const wxString &message = wxDirSelectorPromptStr,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxDIRBTN_DEFAULT_STYLE,
        const wxValidator& validator = wxDefaultValidator,
        const wxString& name = wxDirPickerWidgetNameStr)
        : wxGenericDirButton(parent, id, label, path, message, pos, size,
        style, validator, name)
    {}

    virtual wxDialog *CreateDialog();

private:
    DECLARE_DYNAMIC_CLASS(EightGenericDirButton)

};

class EightDirPickerCtrl
    : public wxDirPickerCtrl
{
public:
    EightDirPickerCtrl() {}

    EightDirPickerCtrl(wxWindow *parent,
        wxWindowID id,
        const wxString& path = wxEmptyString,
        const wxString& message = wxDirSelectorPromptStr,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxDIRP_DEFAULT_STYLE,
        const wxValidator& validator = wxDefaultValidator,
        const wxString& name = wxDirPickerCtrlNameStr)
    {
        Create(parent, id, path, message, pos, size, style, validator, name);
    }

protected:
    virtual
        wxFileDirPickerWidgetBase *CreatePicker(wxWindow *parent,
        const wxString& path,
        const wxString& message,
        const wxString& wildcard)
    {
        return new EightGenericDirButton(parent, wxID_ANY,
            wxGetTranslation(wxDirPickerWidgetLabel),
            path, message,
            wxDefaultPosition, wxDefaultSize,
            GetPickerStyle(GetWindowStyle()));
    }

private:
    DECLARE_DYNAMIC_CLASS(EightDirPickerCtrl)

};

#else

#define EightDirPickerCtrl wxDirPickerCtrl

#endif

#endif // EIGHTFILEPICKER_H
