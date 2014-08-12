#include "stdwx.h"
#include "EightZipFrame.h"

#include "EightZipConfig.h"
#include "ExtractDialog.h"
#include "FileExplorer.h"

#define BIND_MENU_EVENT(functor, id) Bind(wxEVT_MENU, &EightZipFrame::functor, this, id)

EightZipFrame::EightZipFrame()
    : wxFrame(nullptr, wxID_ANY, EIGHTZIP_NAME)
{
    __Create();
    ExtractDialog(nullptr, wxID_ANY, _T("Extract")).ShowModal();
}

EightZipFrame::~EightZipFrame()
{
    __UpdateConfig();
    EightZipConfig::DestroyInstance();
}

void EightZipFrame::__Create()
{
    SetIcon(wxIcon(wxT("ICON_EIGHTZIP")));

    __CreateMenu();

    auto *pSizerMain = new wxBoxSizer(wxVERTICAL);

    __CreateToolBar(pSizerMain);
    __CreateFileExplorer(pSizerMain);

    SetSizer(pSizerMain);

    __RevertSize();
}

void EightZipFrame::__CreateMenu()
{
    auto *pMenuFile = new wxMenu;
    pMenuFile->Append(ID_FILE_OPEN, _("&Open"));
    pMenuFile->AppendSeparator();
    pMenuFile->Append(ID_FILE_EXIT, _("E&xit"));

    auto *pMenuCommand = new wxMenu;
    pMenuCommand->Append(ID_COMMAND_ADD, _("&Add to archive..."));
    pMenuCommand->Append(ID_COMMAND_EXTRACT, _("&Extract files..."));
    
    auto *pMenuBar = new wxMenuBar;
    pMenuBar->Append(pMenuFile, _("&File"));
    pMenuBar->Append(pMenuCommand, _("&Command"));
    SetMenuBar(pMenuBar);

    BIND_MENU_EVENT(__OnFileExitClick, ID_FILE_EXIT);
    BIND_MENU_EVENT(__OnCommandExtractClick, ID_COMMAND_EXTRACT);
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
    m_pFileExplorer = new FileExplorer(this);

    pSizerMain->Add(m_pFileExplorer, wxSizerFlags().Expand().Proportion(1));
}

void EightZipFrame::__RevertSize()
{
    Bind(wxEVT_MOVE, &EightZipFrame::__OnMove, this);
    Bind(wxEVT_SIZE, &EightZipFrame::__OnSize, this);

    auto &config = EightZipConfig::GetInstance();
    SetSize(
        config.Get(ConfigIndex::Width, 1000),
        config.Get(ConfigIndex::Height, 560));
    int nX;
    int nY;
    if (config.Get(ConfigIndex::LocationX, &nX)
        && config.Get(ConfigIndex::LocationY, &nY))
    {
        SetPosition(wxPoint(nX, nY));
    }
    else
    {
        Center();
    }
    Maximize(config.Get(ConfigIndex::IsMaximized, false));
}

void EightZipFrame::__OnFileExitClick(wxCommandEvent &WXUNUSED(event))
{
    Close(TRUE);
}

void EightZipFrame::__OnCommandExtractClick(wxCommandEvent &WXUNUSED(event))
{
    auto entry = m_pFileExplorer->GetEntry(
        m_pFileExplorer->GetSelectedEntryIndex());
    if (!entry)
    {
        return;
    }
    if (!entry->CanExtract())
    {
        wxMessageBox(
            wxString::Format(_("Cannot extract \"%s\"."), entry->GetPath()),
            EIGHTZIP_NAME);
        return;
    }
    ExtractDialog(nullptr, wxID_ANY, _T("Extract")).ShowModal();
}

void EightZipFrame::__OnMove(wxMoveEvent& event)
{
    __UpdateLastLayout();
    event.Skip();
}

void EightZipFrame::__OnSize(wxSizeEvent& event)
{
    __UpdateLastLayout();
    event.Skip();
}

void EightZipFrame::__UpdateLastLayout()
{
    if (!IsIconized())
    {
        m_isLastMaxmized = IsMaximized();
        if (!m_isLastMaxmized)
        {
            m_lastPosition = GetPosition();
            m_lastSize = GetSize();
        }
    }
}

void EightZipFrame::__UpdateConfig()
{
    auto &config = EightZipConfig::GetInstance();
    config.Set(ConfigIndex::LocationX, m_lastPosition.x);
    config.Set(ConfigIndex::LocationY, m_lastPosition.y);
    config.Set(ConfigIndex::Width, m_lastSize.GetWidth());
    config.Set(ConfigIndex::Height, m_lastSize.GetHeight());
    config.Set(ConfigIndex::IsMaximized, m_isLastMaxmized);
}
