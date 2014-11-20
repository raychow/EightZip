#include "stdwx.h"
#include "EightZipFrame.h"

#include "SevenZipCore/Exception.h"

#include "ArchiveHelper.h"
#include "EightZipConfig.h"
#include "ExtractDialog.h"
#include "FileExplorer.h"

#define BIND_MENU_EVENT(functor, id) \
    Bind(wxEVT_MENU, &EightZipFrame::functor, this, id)

EightZipFrame::EightZipFrame()
    : wxFrame(nullptr, wxID_ANY, EIGHTZIP_NAME)
    , m_windowStateManager(this,
    ConfigIndex::EightZipLocationX,
    ConfigIndex::EightZipLocationY,
    ConfigIndex::EightZipWidth,
    ConfigIndex::EightZipHeight,
    ConfigIndex::EightZipIsMaximized)
{
    __Create();
}

void EightZipFrame::__Create()
{
    SetIcon(wxIcon(wxT("ICON_EIGHTZIP")));

    __CreateMenu();

    auto *pSizerMain = new wxBoxSizer(wxVERTICAL);

    __CreateToolBar(pSizerMain);
    __CreateFileExplorer(pSizerMain);

    SetSizer(pSizerMain);
    m_windowStateManager.LoadState();

    // Get rid of any focus.
    SetFocus();
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

void EightZipFrame::__OnFileExitClick(wxCommandEvent &WXUNUSED(event))
{
    Close(TRUE);
}

void EightZipFrame::__OnCommandExtractClick(wxCommandEvent &WXUNUSED(event))
{
    auto vEntry = m_pFileExplorer->GetSelectedEntries();
    auto spModel = m_pFileExplorer->GetModel();
    auto spFolderModel = spModel;
    TString tstrExtractFolder;
    if (spModel->IsArchive())
    {
        auto spVirtualFolderModel = dynamic_pointer_cast<
            VirtualModel>(spFolderModel);
        while (!spVirtualFolderModel->IsRoot())
        {
            spVirtualFolderModel = dynamic_pointer_cast<VirtualModel>(
                spVirtualFolderModel->GetParent());
        }
        tstrExtractFolder = SevenZipCore::Helper::GetFileNameStem(
            spVirtualFolderModel->GetName());
        spFolderModel = spVirtualFolderModel->GetParent();
    }
    else
    {
        if (vEntry.empty())
        {
            wxMessageBox(
                wxString::Format(_("No file selected."), EIGHTZIP_NAME));
            return;
        }
        for (auto &entry : vEntry)
        {
            if (entry.get().IsDirectory())
            {
                wxMessageBox(wxString::Format(_(
                    "You must select one or more files."), EIGHTZIP_NAME));
                return;
            }
        }
        if (vEntry.size() == 1)
        {
            tstrExtractFolder = SevenZipCore::Helper::GetFileNameStem(
                vEntry.front().get().GetName());
        }
        else
        {
            tstrExtractFolder = spModel->GetName();
        }
    }
    ExtractDialog dialog(this, wxID_ANY, _T("Extract"));
    dialog.SetPath(SevenZipCore::Helper::MakePathSlash(
        spFolderModel->GetPath()) + tstrExtractFolder);
    dialog.CenterOnParent();
    if (dialog.ShowModal() != wxID_OK)
    {
        return;
    }

    auto extractPath = boost::filesystem::absolute(
        dialog.GetPath(), spFolderModel->GetPath()).wstring();
    if (vEntry.empty())
    {
        // spModel is a VirtualModel.
        auto spVirtualModel = dynamic_pointer_cast<VirtualModel>(spModel);
        if (!Helper::Extract(extractPath,
            spVirtualModel->GetInternalLocation(),
            spVirtualModel,
            dialog.IsLaunchFolder()))
        {
            wxMessageBox(_("Extract failed."));
        }
    }
    else
    {
        if (!Helper::Extract(
            extractPath,
            spModel->IsArchive()
            ? dynamic_pointer_cast<VirtualModel>(spModel)->GetInternalLocation()
            : wxEmptyString,
            vEntry,
            dialog.IsLaunchFolder()))
        {
            wxMessageBox(_("Extract failed."));
        }
    }
}
