// EightDirDialog.h

#ifndef EIGHTDIRDIALOG_H
#define EIGHTDIRDIALOG_H

#ifdef __WXMSW__

class EightDirDialog
    : public wxDirDialog
{
public:
    EightDirDialog(wxWindow *parent,
        const wxString& message = wxDirSelectorPromptStr,
        const wxString& defaultPath = wxEmptyString,
        long style = wxDD_DEFAULT_STYLE,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        const wxString& name = wxDirDialogNameStr)
        : wxDirDialog(parent, message, defaultPath, style, pos, size, name)
    {}

    virtual int ShowModal();

private:
    int __ShowIFileDialog(WXHWND owner);

};

#else

#define EightDirDialog DirDialog

#endif

#endif // EIGHTDIRDIALOG_H
