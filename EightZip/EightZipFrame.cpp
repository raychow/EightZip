#include "stdwx.h"
#include "EightZipFrame.h"

#include "FileExplorer.h"

EightZipFrame::EightZipFrame(const wxString &title)
    : wxFrame(nullptr, wxID_ANY, title)
    , m_auiManager(this)
{
    __Create();
}

EightZipFrame::~EightZipFrame()
{
    m_auiManager.UnInit();
}

void EightZipFrame::__Create()
{
    __CreateMenu();
    __CreateToolBar();

    m_auiManager.AddPane(
        new FileExplorer(this),
        wxAuiPaneInfo().Name(wxT("FileExplorer")).CenterPane());
    m_auiManager.Update();
}

void EightZipFrame::__CreateMenu()
{
    auto *pMenuFile = new wxMenu;
    pMenuFile->Append(ID_OPEN, _("&Open"));
    pMenuFile->AppendSeparator();
    pMenuFile->Append(ID_EXIT, _("E&xit"));
    
    auto *pMenuBar = new wxMenuBar;
    pMenuBar->Append(pMenuFile, _("&File"));
    SetMenuBar(pMenuBar);

    pMenuFile->Bind(
        wxEVT_COMMAND_MENU_SELECTED,
        &EightZipFrame::__OnExitClick,
        this,
        ID_EXIT);
}

void EightZipFrame::__CreateToolBar()
{
    auto *pToolBar = new wxToolBar(this, wxID_ANY);
    m_auiManager.AddPane(
        pToolBar,
        wxAuiPaneInfo().Caption("ToolBar").Name(wxT("ToolBar")).ToolbarPane().Top());
}

void EightZipFrame::__OnExitClick(wxCommandEvent &WXUNUSED(event))
{
    Close(TRUE);
}
