// ExplorerListCtrl.h

#ifndef EXPLORERLISTCTRL_H
#define EXPLORERLISTCTRL_H

class ExplorerListCtrl :
    public wxListCtrl
{
public:
    ExplorerListCtrl(wxWindow *parent,
        wxWindowID id = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxLC_ICON,
        const wxValidator& validator = wxDefaultValidator,
        const wxString& name = wxListCtrlNameStr);
    virtual ~ExplorerListCtrl();

private:
    DECLARE_EVENT_TABLE()
    wxDECLARE_NO_COPY_CLASS(ExplorerListCtrl);
};


#endif // EXPLORERLISTCTRL_H