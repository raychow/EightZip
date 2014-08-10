#include "stdwx.h"
#include "EightZipFrame.h"

#include "FileExplorer.h"

EightZipFrame::EightZipFrame(const wxString &title)
    : wxFrame(nullptr, wxID_ANY, title)
{
    __Create();
}

EightZipFrame::~EightZipFrame()
{
}

void EightZipFrame::__Create()
{
    SetIcon(wxIcon(wxT("ICON_EIGHTZIP")));

    __CreateMenu();

    auto *pSizerMain = new wxBoxSizer(wxVERTICAL);

    __CreateToolBar(pSizerMain);
    __CreateFileExplorer(pSizerMain);

    SetSizer(pSizerMain);
}

void EightZipFrame::__CreateMenu()
{
    auto *pMenuFile = new wxMenu;
    pMenuFile->Append(ID_FILE_OPEN, _("&Open"));
    pMenuFile->AppendSeparator();
    pMenuFile->Append(ID_FILE_EXIT, _("E&xit"));
    
    auto *pMenuBar = new wxMenuBar;
    pMenuBar->Append(pMenuFile, _("&File"));
    SetMenuBar(pMenuBar);

    pMenuFile->Bind(
        wxEVT_COMMAND_MENU_SELECTED,
        &EightZipFrame::__OnExitClick,
        this,
        ID_FILE_EXIT);
}

void EightZipFrame::__CreateToolBar(wxBoxSizer *pSizerMain)
{
    auto *pToolBar = CreateToolBar();
    pToolBar->SetWindowStyle(wxTB_NODIVIDER);
    pToolBar->SetToolBitmapSize(wxSize(48, 48));
    pToolBar->AddTool(ID_COMMAND_ADD, _T("Add"), wxBITMAP_PNG(PNG_ADD));
    pToolBar->AddTool(ID_COMMAND_EXTRACT, _T("Extract"), wxBITMAP_PNG(PNG_EXTRACT));
    pToolBar->Realize();

    pSizerMain->Add(pToolBar, wxSizerFlags().Expand().Proportion(0));
}

void EightZipFrame::__CreateFileExplorer(wxBoxSizer *pSizerMain)
{
    auto *pFileExplorer = new FileExplorer(this);

    pSizerMain->Add(pFileExplorer, wxSizerFlags().Expand().Proportion(1));
}

void EightZipFrame::__OnExitClick(wxCommandEvent &WXUNUSED(event))
{
    Close(TRUE);
}
