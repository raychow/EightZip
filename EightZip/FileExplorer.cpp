#include "stdwx.h"
#include "FileExplorer.h"

#include "SevenZipCore/Exception.h"

#include "Exception.h"
#include "FolderModel.h"

using namespace std;

FileExplorer::FileExplorer(
    wxWindow *parent,
    wxWindowID winid /*= wxID_ANY*/,
    const wxPoint &pos /*= wxDefaultPosition*/,
    const wxSize &size /*= wxDefaultSize*/,
    long style /*= wxTAB_TRAVERSAL | wxNO_BORDER*/,
    const wxString &name /*= wxPanelNameStr*/)
    : wxPanel(parent, winid, pos, size, style, name)
{
    __Create();
}

void FileExplorer::NavigateTo(TString tstrPath)
{
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
    m_pToolBar->AddTool(
        ID_PARENT_FOLDER, _("Parent Folder"), wxICON16(ICON_PARENT_FOLDER));
    m_pToolBar->Realize();
    pSizerPath->Add(m_pToolBar, wxSizerFlags().Center());
    m_pComboBox = new wxComboBox(this, wxID_ANY);
    m_pComboBox->SetWindowStyle(wxTE_PROCESS_ENTER);
    pSizerPath->Add(
        m_pComboBox, wxSizerFlags().Proportion(1).Border(wxLEFT, 3));

    pSizerMain->Add(pSizerPath, wxSizerFlags().Expand().Proportion(0));

    m_pToolBar->Bind(
        wxEVT_TOOL,
        &FileExplorer::__OnParentFolderClick,
        this,
        ID_PARENT_FOLDER);
    m_pComboBox->Bind(
        wxEVT_TEXT_ENTER,
        &FileExplorer::__OnPathComboBoxEnter,
        this);
}

void FileExplorer::__CreateExplorer(wxBoxSizer *pSizerMain)
{
    // TODO: Catch exception.
    m_pListCtrl = new FileListCtrl(this);
    __SetModel(make_shared<FolderModel>(wxT("D:\\Test\\")));
    
    pSizerMain->Add(m_pListCtrl, wxSizerFlags().Expand().Proportion(1));

    m_pListCtrl->Bind(
        wxEVT_LIST_ITEM_ACTIVATED, &FileExplorer::__OnListItemActivated, this);
}

void FileExplorer::__SetModel(std::shared_ptr<IModel> spModel)
{
    m_pListCtrl->SetModel(spModel);
    m_spModel = move(spModel);
    auto tstrPath = m_spModel->GetPath();
    if (tstrPath.empty() || !wxIsPathSeparator(tstrPath.back()))
    {
        tstrPath.push_back(wxFILE_SEP_PATH);
    }
    m_pComboBox->SetValue(tstrPath);
}

void FileExplorer::__OnParentFolderClick(wxCommandEvent &event)
{
    if (m_spModel->HasParent())
    {
        __SetModel(m_spModel->GetParent());
    }
    if (!m_spModel->HasParent())
    {
        m_pToolBar->EnableTool(ID_PARENT_FOLDER, false);
    }
}

void FileExplorer::__OnPathComboBoxEnter(wxCommandEvent &event)
{
}

void FileExplorer::__OnListItemActivated(wxListEvent &event)
{
    const auto &spEntry = m_spModel->GetEntries().at(
        m_pListCtrl->GetModelIndex(event.GetIndex()));
    if (!spEntry->IsOpenExternal())
    {
        try
        {
            __SetModel(spEntry->GetModel());
            m_pToolBar->EnableTool(ID_PARENT_FOLDER, true);
            return;
        }
        catch (const ModelException &)
        {
            wxMessageBox(
                wxString::Format(_("Cannot access \"%s\"."),
                spEntry->GetPath()),
                EIGHT_ZIP_NAME,
                wxOK | wxICON_ERROR);
            return;
        }
        catch (const std::exception &)
        {
            // Open external.
        }
    }
    // else
    try
    {
        spEntry->OpenExternal();
    }
    catch (const SystemException &)
    {
        wxMessageBox(
            wxString::Format(_("Cannot open \"%s\"."),
            spEntry->GetPath()),
            EIGHT_ZIP_NAME,
            wxOK | wxICON_ERROR);
    }
}
