#include "stdwx.h"
#include "FileExplorer.h"

#include <boost/filesystem.hpp>

#include "SevenZipCore/Exception.h"

#include "ArchiveHelper.h"
#include "EightZipConfig.h"
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

FileExplorer::~FileExplorer()
{
    auto &config = EightZipConfig::GetInstance();
    config.Set(ConfigIndex::Path, m_spModel->GetPath());
}

void FileExplorer::NavigateTo(TString tstrPath)
{
    __SetModel(GetModelFromPath(m_spModel, tstrPath));
}

int FileExplorer::GetSelectedIndex() const
{
    return m_pListCtrl->GetNextItem(-1,
        wxLIST_NEXT_ALL,
        wxLIST_STATE_SELECTED);
}

vector<int> FileExplorer::GetSelectedIndexes() const
{
    int index = -1;
    vector<int> result;

    while (true)
    {
        index = m_pListCtrl->GetNextItem(index,
            wxLIST_NEXT_ALL,
            wxLIST_STATE_SELECTED);
        if (index == -1)
        {
            return result;
        }
        result.push_back(index);
    }
}

EntryBase &FileExplorer::GetSelectedEntry() const
{
    return GetEntry(m_pListCtrl->GetEntryIndex(GetSelectedIndex()));
}

vector<reference_wrapper<EntryBase>> FileExplorer::GetSelectedEntries() const
{
    vector<reference_wrapper<EntryBase>> result;
    vector<int> indexes = GetSelectedIndexes();
    for (auto index : indexes)
    {
        result.push_back(GetEntry(m_pListCtrl->GetEntryIndex(index)));
    }
    return result;
}

EntryBase &FileExplorer::GetEntry(int nIndex) const
{
    return (*m_spModel)[nIndex];
}

void FileExplorer::__Create()
{
    auto *pSizerMain = new wxBoxSizer(wxVERTICAL);

    __CreateAddressBar(pSizerMain);
    __CreateExplorer(pSizerMain);

    SetSizer(pSizerMain);
}

void FileExplorer::__CreateAddressBar(wxBoxSizer *pSizerMain)
{
    auto *pSizerAddress = new wxBoxSizer(wxHORIZONTAL);
    m_pParentFolderToolBar = new wxToolBar(this, wxID_ANY);
    m_pParentFolderToolBar->SetWindowStyle(wxTB_DEFAULT_STYLE | wxTB_NODIVIDER);
    m_pParentFolderToolBar->AddTool(
        ID_PARENT_FOLDER, _("Parent Folder"), wxICON16(ICON_PARENT_FOLDER));
    m_pParentFolderToolBar->Realize();
    pSizerAddress->Add(m_pParentFolderToolBar, wxSizerFlags().Center());
    m_pAddressComboBox = new wxComboBox(this, wxID_ANY);
    m_pAddressComboBox->AutoCompleteFileNames();
    m_pAddressComboBox->Bind(
        wxEVT_KEY_DOWN, &FileExplorer::__OnPathComboBoxKeyDown, this);
    m_pAddressComboBox->SetWindowStyle(wxTE_PROCESS_ENTER);
    pSizerAddress->Add(
        m_pAddressComboBox, wxSizerFlags().Proportion(1).Border(wxALL, 2));

    pSizerMain->Add(pSizerAddress, wxSizerFlags().Expand().Proportion(0));

    m_pParentFolderToolBar->Bind(
        wxEVT_TOOL,
        &FileExplorer::__OnParentFolderClick,
        this,
        ID_PARENT_FOLDER);
    //m_pAddressComboBox->Bind(
    //    wxEVT_TEXT_ENTER,
    //    &FileExplorer::__OnPathComboBoxEnter,
    //    this);
}

void FileExplorer::__CreateExplorer(wxBoxSizer *pSizerMain)
{
    // TODO: Catch exception.
    m_pListCtrl = new FileListCtrl(this);
    auto &config = EightZipConfig::GetInstance();
    __SetModel(GetModelFromPath(config.GetString(ConfigIndex::Path), false));
    
    pSizerMain->Add(m_pListCtrl, wxSizerFlags().Expand().Proportion(1));

    m_pListCtrl->Bind(
        wxEVT_LIST_ITEM_ACTIVATED, &FileExplorer::__OnListItemActivated, this);
}

void FileExplorer::__SetModel(shared_ptr<ModelBase> spModel,
    TString tstrFocusedName/* = wxEmptyString*/)
{
    if (!spModel)
    {
        return;
    }
    m_pListCtrl->SetModel(*spModel, tstrFocusedName);
    auto tstrPath = spModel->GetPath();
    m_pAddressComboBox->SetValue(tstrPath);
    m_pAddressComboBox->SelectAll();
    m_pParentFolderToolBar->EnableTool(ID_PARENT_FOLDER, spModel->HasParent());
    m_spModel = move(spModel);
}

void FileExplorer::__OnParentFolderClick(wxCommandEvent &event)
{
    if (m_spModel->HasParent())
    {
        __SetModel(m_spModel->GetParent(), m_spModel->GetName());
    }
}

void FileExplorer::__OnPathComboBoxKeyDown(wxKeyEvent& event)
{
    if (event.GetKeyCode() == WXK_RETURN)
    {
        NavigateTo(m_pAddressComboBox->GetValue().ToStdWstring());
    }
    event.Skip();
}

void FileExplorer::__OnListItemActivated(wxListEvent &event)
{
    const auto &entry = GetEntry(m_pListCtrl->GetEntryIndex(event.GetIndex()));
    if (!entry.IsOpenExternal())
    {
        try
        {
            __SetModel(entry.GetModel());
            return;
        }
        catch (const ArchiveException &)
        {
            return;
        }
        catch (const SevenZipCore::ArchiveException &ex)
        {
            if (ex.GetErrorCode() == E_ABORT)
            {
                return;
            }
        }
        catch (const exception &)
        {
            // Open external.
        }
    }
    // else
    try
    {
        entry.OpenExternal();
    }
    catch (const SystemException &)
    {
        wxMessageBox(
            wxString::Format(_("Cannot open \"%s\"."),
            entry.GetPath()),
            EIGHTZIP_NAME,
            wxOK | wxICON_ERROR);
    }
}
