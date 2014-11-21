#include "stdwx.h"
#include "ArchiveHelper.h"

#include <queue>
#include <thread>

#include "SevenZipCore/ArchiveFile.h"
#include "SevenZipCore/CommonHelper.h"
#include "SevenZipCore/Exception.h"

#include "EntryBase.h"
#include "ExtractIndicator.h"
#include "Extractor.h"
#include "FileHelper.h"
#include "FolderEntry.h"
#include "ProgressDialog.h"
#include "VirtualEntry.h"
#include "VirtualModel.h"

using namespace std;

namespace Helper
{
    static void CallExecute(Extractor &extractor,
        ProgressDialog *pProgressDialog,
        bool isLaunchFolder)
    {
        try
        {
            ProgressDialogManager dialogManager(pProgressDialog);
            extractor.Execute();
            dialogManager.SetSuccess(true);
        }
        catch (const SevenZipCore::ArchiveException &)
        {
            return;
        }
        if (isLaunchFolder)
        {
            try
            {
                OpenFileExternal(extractor.GetExtractLocation());
            }
            catch (const SystemException &)
            {
                wxMessageBox(
                    wxString::Format(_("Cannot open \"%s\"."),
                    extractor.GetExtractLocation()),
                    EIGHTZIP_NAME,
                    wxOK | wxICON_ERROR);
            }
        }
    }

    static void ExtractFolderEntriesThread(TString tstrExtractPath,
        vector<reference_wrapper<FolderEntry>> vEntry,
        ProgressDialog *pProgressDialog,
        bool isLaunchFolder)
    {
        ExtractIndicator extractIndicator(pProgressDialog);

        RealFileExtractor extractor(tstrExtractPath, &extractIndicator);
        for (auto &entry : vEntry)
        {
            extractor.AddPlan(entry.get().GetPath());
        }
        CallExecute(extractor, pProgressDialog, isLaunchFolder);
    }

    static void ExtractVirtualModelThread(TString tstrExtractPath,
        TString tstrInternalPath,
        shared_ptr<VirtualModel> spModel,
        ProgressDialog *pProgressDialog,
        bool isLaunchFolder)
    {
        ExtractIndicator extractIndicator(pProgressDialog);

        VirtualFileExtractor extractor(tstrExtractPath,
            &extractIndicator,
            tstrInternalPath,
            spModel->GetPath(),
            spModel->GetArchiveFolder().GetArchiveEntry());
        extractor.AddPlan(*spModel);
        CallExecute(extractor, pProgressDialog, isLaunchFolder);
    }

    static void ExtractVirtualEntriesThread(TString tstrExtractPath,
        TString tstrInternalPath,
        vector<reference_wrapper<VirtualEntry>> vEntry,
        ProgressDialog *pProgressDialog,
        bool isLaunchFolder)
    {
        if (vEntry.empty())
        {
            return;
        }

        ExtractIndicator extractIndicator(pProgressDialog);

        auto spModel = dynamic_pointer_cast<VirtualModel>(
            vEntry.front().get().GetContainer());
        while (!spModel->IsRoot())
        {
            spModel = dynamic_pointer_cast<VirtualModel>(spModel->GetParent());
        }

        VirtualFileExtractor extractor(tstrExtractPath,
            &extractIndicator,
            tstrInternalPath,
            spModel->GetPath(),
            vEntry.front().get().GetArchiveFile().GetArchiveEntry());
        for (auto &entry : vEntry)
        {
            extractor.AddPlan(entry);
        }
        CallExecute(extractor, pProgressDialog, isLaunchFolder);
    }

    bool Extract(TString tstrPath,
        TString tstrInternalPath,
        shared_ptr<VirtualModel> spModel,
        bool isLaunchFolder)
    {
        try
        {
            if (!spModel)
            {
                return false;
            }
            boost::filesystem::create_directories(tstrPath);

            auto *pProgressDialog = new ProgressDialog(wxTheApp->GetTopWindow(),
                wxID_ANY,
                ProgressDialog::Mode::Extract);

            thread extractThread(ExtractVirtualModelThread,
                Helper::GetCanonicalPath(tstrPath), tstrInternalPath,
                move(spModel), pProgressDialog, isLaunchFolder);
            extractThread.detach();
        }
        catch (const boost::system::system_error &)
        {
            return false;
        }
        return true;
    }

    bool Extract(TString tstrPath,
        TString tstrInternalPath,
        const vector<reference_wrapper<EntryBase>> &vEntry,
        bool isLaunchFolder)
    {
        try
        {
            if (vEntry.empty())
            {
                return false;
            }
            boost::filesystem::create_directories(tstrPath);

            auto *pProgressDialog = new ProgressDialog(wxTheApp->GetTopWindow(),
                wxID_ANY,
                ProgressDialog::Mode::Extract);

            if (vEntry.front().get().IsVirtual())
            {
                auto vVirtualEntry = vector < reference_wrapper<VirtualEntry> > { };
                for (const auto &entry : vEntry)
                {
                    vVirtualEntry.push_back(
                        dynamic_cast<VirtualEntry &>(entry.get()));
                }
                thread { ExtractVirtualEntriesThread,
                    Helper::GetCanonicalPath(tstrPath),
                    tstrInternalPath,
                    move(vVirtualEntry),
                    pProgressDialog,
                    isLaunchFolder }.detach();
            }
            else
            {
                auto vFolderEntry = vector < reference_wrapper<FolderEntry> > { };
                for (const auto &entry : vEntry)
                {
                    vFolderEntry.push_back(
                        dynamic_cast<FolderEntry &>(entry.get()));
                }
                thread { ExtractFolderEntriesThread,
                    Helper::GetCanonicalPath(tstrPath),
                    move(vFolderEntry),
                    pProgressDialog,
                    isLaunchFolder }.detach();
            }
        }
        catch (const boost::system::system_error &)
        {
            return false;
        }
        catch (const bad_cast &)
        {
            return false;
        }
        return true;
    }
}
