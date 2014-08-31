#include "stdwx.h"
#include "FileExplorer.h"

#include <boost/filesystem.hpp>

#include "SevenZipCore/Exception.h"

#include "EightZipConfig.h"
#include "Exception.h"
#include "FolderModel.h"
#include "Helper.h"

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

std::vector<int> FileExplorer::GetSelectedIndexes() const
{
    int index = -1;
    vector<int> result;

    while (true)
    {
        index = m_pListCtrl->GetNextItem(index,
            wxLIST_NEXT_ALL,
            wxLIST_STATE_SELECTED);
        result.push_back(index);

        if (index == -1)
        {
            return result;
        }
    }
}

int FileExplorer::GetSelectedEntryIndex() const
{
    return m_pListCtrl->GetEntryIndex(GetSelectedIndex());
}

std::vector<int> FileExplorer::GetSelectedEntryIndexes() const
{
    auto result = GetSelectedIndexes();
    for (auto &index : result)
    {
        index = m_pListCtrl->GetEntryIndex(index);
    }
    return result;
}

std::shared_ptr<IEntry> FileExplorer::GetSelectedEntry() const
{
    return GetEntry(GetSelectedEntryIndex());
}

std::shared_ptr<IEntry> FileExplorer::GetEntry(int nIndex) const
{
    if (0 > nIndex || !m_spModel)
    {
        return nullptr;
    }
    auto entries = m_spModel->GetEntries();
    if (nIndex > static_cast<int>(entries.size()))
    {
        return nullptr;
    }
    return entries.at(nIndex);
}

bool FileExplorer::CanExtract() const
{
    if (m_spModel->IsArchive())
    {
        return true;
    }
    auto entry = GetEntry(GetSelectedEntryIndex());
    return entry && entry->CanExtract();
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

void FileExplorer::__SetModel(std::shared_ptr<IModel> spModel)
{
    m_pListCtrl->SetModel(spModel);
    m_spModel = move(spModel);
    auto tstrPath = m_spModel->GetPath();
    m_pAddressComboBox->SetValue(tstrPath);
    m_pAddressComboBox->SelectAll();
    m_pParentFolderToolBar->EnableTool(ID_PARENT_FOLDER, m_spModel->HasParent());
}

void FileExplorer::__OnParentFolderClick(wxCommandEvent &event)
{
    if (m_spModel->HasParent())
    {
        __SetModel(m_spModel->GetParent());
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
    const auto spEntry = GetEntry(m_pListCtrl->GetEntryIndex(event.GetIndex()));
    if (!spEntry->IsOpenExternal())
    {
        try
        {
            __SetModel(spEntry->GetModel());
            return;
        }
        catch (const ModelException &)
        {
            wxMessageBox(
                wxString::Format(_("Cannot access \"%s\"."),
                spEntry->GetPath()),
                EIGHTZIP_NAME,
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
            EIGHTZIP_NAME,
            wxOK | wxICON_ERROR);
    }
}

void FileExplorer::Extract(TString tstrPath)
{
    if (!m_spModel->IsArchive())
    {
        wxMessageBox(_("Extract failed."));
        return;
    }
    Extract(tstrPath, dynamic_pointer_cast<ArchiveModel>(m_spModel));
}

void FileExplorer::Extract(TString tstrPath, shared_ptr<ArchiveModel> spModel)
{
    try
    {
        if (!spModel || !spModel->IsArchive())
        {
            wxMessageBox(_("Extract failed."));
            return;
        }
        shared_ptr<IModel> spCurrentModel = spModel;
        while (spCurrentModel && spCurrentModel->IsParentArchive())
        {
            spCurrentModel = spCurrentModel->GetParent();
        }
        assert(spCurrentModel);
        auto tstrAbsPath = spCurrentModel->GetParentPath();
        auto path = boost::filesystem::absolute(tstrPath, tstrAbsPath);
        boost::filesystem::create_directories(path);
        auto tstrCanonicalPath = Helper::GetCanonicalPath(path.wstring());
        dynamic_pointer_cast<ArchiveModel>(spModel)->Extract(tstrPath);
    }
    catch (const boost::system::system_error &)
    {
        wxMessageBox(wxString::Format(
            _("Can not access or create folder \"%s\"."),
            tstrPath));
    }
}
