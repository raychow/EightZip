#include "stdwx.h"
#include "FileExplorer.h"

#include "Exception.h"
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
    m_pToolBar = new wxToolBar(this, wxID_ANY);
    m_pToolBar->SetWindowStyle(wxTB_DEFAULT_STYLE | wxTB_NODIVIDER);
    m_pToolBar->AddTool(ID_PARENT_FOLDER, _("Parent Folder"), wxICON16(ICON_PARENT_FOLDER));
    m_pToolBar->Realize();
    pSizerPath->Add(m_pToolBar, wxSizerFlags().Center());
    pSizerPath->Add(new wxComboBox(this, wxID_ANY), wxSizerFlags().Proportion(1).Border(wxLEFT, 3));

    pSizerMain->Add(pSizerPath, wxSizerFlags().Expand().Proportion(0));

    m_pToolBar->Bind(wxEVT_TOOL, &FileExplorer::__OnParentFolderClick, this, ID_PARENT_FOLDER);
}

void FileExplorer::__CreateExplorer(wxBoxSizer *pSizerMain)
{
    // TODO: Catch exception.
    m_spModel = make_shared<FileSystemModel>(wxT("C:"));
    m_pListCtrl = new FileListCtrl(this);
    m_pListCtrl->SetModel(m_spModel);
    
    pSizerMain->Add(m_pListCtrl, wxSizerFlags().Expand().Proportion(1));

    m_pListCtrl->Bind(wxEVT_LIST_ITEM_ACTIVATED, &FileExplorer::__OnListItemActivated, this);
}

void FileExplorer::__OnParentFolderClick(wxCommandEvent &event)
{
    if (m_spModel->HasParent())
    {
        m_spModel = m_spModel->GetParent();
        m_pListCtrl->SetModel(m_spModel);
    }
    if (!m_spModel->HasParent())
    {
        m_pToolBar->EnableTool(ID_PARENT_FOLDER, false);
    }
}

void FileExplorer::__OnListItemActivated(wxListEvent &event)
{
    const auto &spEntry = m_spModel->GetEntries().at(m_pListCtrl->GetModelIndex(event.GetIndex()));
    try
    {
        m_spModel = spEntry->GetModel();
        m_pListCtrl->SetModel(m_spModel);
        m_pToolBar->EnableTool(ID_PARENT_FOLDER, true);
    }
    catch (ModelException)
    {
        wxMessageBox(_("Cannot access \"" + spEntry->GetFullPath() + "\"."));
    }
}
