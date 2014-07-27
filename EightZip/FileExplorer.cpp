#include "stdwx.h"
#include "FileExplorer.h"

#include "FileSystemModel.h"

using namespace std;

FileExplorer::FileExplorer(wxWindow *parent, wxWindowID winid /*= wxID_ANY*/, const wxPoint &pos /*= wxDefaultPosition*/, const wxSize &size /*= wxDefaultSize*/, long style /*= wxTAB_TRAVERSAL | wxNO_BORDER*/, const wxString &name /*= wxPanelNameStr*/)
    : wxPanel(parent, winid, pos, size, style, name)
{
    __Create();
}

void FileExplorer::__Create()
{
    auto *pSizerMain = new wxBoxSizer(wxVERTICAL);

    __CreatePath(pSizerMain);
    __CreateExplorer(pSizerMain);

    SetSizer(pSizerMain);
}

void FileExplorer::__CreatePath(wxBoxSizer *pSizerMain)
{
    auto *pSizerPath = new wxBoxSizer(wxHORIZONTAL);
    auto pToolBar = new wxToolBar(this, wxID_ANY);
    pToolBar->SetWindowStyle(wxTB_DEFAULT_STYLE | wxTB_NODIVIDER);
	pToolBar->AddTool(ID_PARENT_FOLDER, _("Parent Folder"), wxICON16(ICON_PARENT_FOLDER));
    pToolBar->Realize();
    pSizerPath->Add(pToolBar, wxSizerFlags().Center());
    pSizerPath->Add(new wxComboBox(this, wxID_ANY), wxSizerFlags().Proportion(1).Border(wxLEFT, 3));

    pSizerMain->Add(pSizerPath, wxSizerFlags().Expand().Proportion(0));

	pToolBar->Bind(wxEVT_TOOL, &FileExplorer::__OnParentFolderClick, this, ID_PARENT_FOLDER);
}

void FileExplorer::__CreateExplorer(wxBoxSizer *pSizerMain)
{
    m_spModel = make_shared<FileSystemModel>(wxT("D:"));
    m_pListCtrl = new FileListCtrl(this);
    m_pListCtrl->SetModel(m_spModel);
    
    pSizerMain->Add(m_pListCtrl, wxSizerFlags().Expand().Proportion(1));

    m_pListCtrl->Bind(wxEVT_LIST_ITEM_ACTIVATED, &FileExplorer::__OnListItemActivated, this);
}

void FileExplorer::__OnParentFolderClick(wxCommandEvent &WXUNUSED(event))
{
    m_spModel = make_shared<FileSystemModel>(m_spModel->GetPath());
	m_pListCtrl->SetModel(m_spModel);
}

void FileExplorer::__OnListItemActivated(wxListEvent &event)
{
	m_spModel = m_spModel->GetChildren().at(m_pListCtrl->GetModelIndex(event.GetIndex()));
	m_pListCtrl->SetModel(m_spModel);
}
