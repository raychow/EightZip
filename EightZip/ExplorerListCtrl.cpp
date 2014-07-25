#include "stdwx.h"
#include "ExplorerListCtrl.h"

BEGIN_EVENT_TABLE(ExplorerListCtrl, wxListCtrl)
END_EVENT_TABLE()


ExplorerListCtrl::ExplorerListCtrl(wxWindow *parent, wxWindowID id /*= wxID_ANY*/, const wxPoint& pos /*= wxDefaultPosition*/, const wxSize& size /*= wxDefaultSize*/, long style /*= wxLC_ICON*/, const wxValidator& validator /*= wxDefaultValidator*/, const wxString& name /*= wxListCtrlNameStr*/)
    : wxListCtrl(parent, id, pos, size, style, validator, name)
{

}


ExplorerListCtrl::~ExplorerListCtrl()
{
}
