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
    class ProgressDialogManager
    {
    public:
        ProgressDialogManager(ProgressDialog *pProgressDialog)
            : m_pDialog(pProgressDialog)
        {
            wxTheApp->CallAfter(bind(
                // Probably be called after destruct.
                [](ProgressDialog *pDialog) {
                pDialog->CenterOnParent();
                pDialog->ShowModal();
            }, m_pDialog));
        }

        ~ProgressDialogManager()
        {
            wxTheApp->CallAfter(bind(
                // Probably be called after destruct.
                [](ProgressDialog *pDialog, bool isSuccess) {
                pDialog->Done(isSuccess);
            }, m_pDialog, m_isSuccess));
        }

        void SetSuccess(bool isSuccess)
        {
            m_isSuccess = isSuccess;
        }

    private:
        ProgressDialog *m_pDialog = nullptr;
        bool m_isSuccess = false;

        ProgressDialogManager(const ProgressDialogManager&) = delete;
        ProgressDialogManager &operator=(const ProgressDialogManager&) = delete;

    };

    static void CallExecute(Extractor &extractor,
        ProgressDialogManager &dialogManager,
        bool isLaunchFolder)
    {
        try
        {
            extractor.Execute();
        }
        catch (const SevenZipCore::ArchiveException &)
        {
            return;
        }
        dialogManager.SetSuccess(true);
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
        ProgressDialogManager dialogManager(pProgressDialog);
        ExtractIndicator extractIndicator(pProgressDialog);

        RealFileExtractor extractor(tstrExtractPath, &extractIndicator);
        for (auto &entry : vEntry)
        {
            extractor.AddPlan(entry.get().GetPath());
        }
        CallExecute(extractor, dialogManager, isLaunchFolder);
    }

    static void ExtractVirtualModelThread(TString tstrExtractPath,
        TString tstrInternalPath,
        shared_ptr<VirtualModel> spModel,
        ProgressDialog *pProgressDialog,
        bool isLaunchFolder)
    {
        ProgressDialogManager dialogManager(pProgressDialog);
        ExtractIndicator extractIndicator(pProgressDialog);

        VirtualFileExtractor extractor(tstrExtractPath,
            &extractIndicator,
            tstrInternalPath,
            spModel->GetPath(),
            spModel->GetArchiveFolder().GetArchiveEntry());
        extractor.AddPlan(*spModel);
        CallExecute(extractor, dialogManager, isLaunchFolder);
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

        ProgressDialogManager dialogManager(pProgressDialog);
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
        CallExecute(extractor, dialogManager, isLaunchFolder);
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

            auto *pProgressDialog = new ProgressDialog(
                wxTheApp->GetTopWindow(), wxID_ANY, _("Extracting"));

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

            auto *pProgressDialog = new ProgressDialog(
                wxTheApp->GetTopWindow(), wxID_ANY, _("Extracting"));

            if (vEntry.front().get().IsVirtual())
            {
                auto vVirtualEntry = vector < reference_wrapper<VirtualEntry> > { };
                for (const auto &entry : vEntry)
                {
                    vVirtualEntry.push_back(
                        dynamic_cast<VirtualEntry &>(entry.get()));
                }
                thread extractThread(ExtractVirtualEntriesThread,
                    Helper::GetCanonicalPath(tstrPath), tstrInternalPath,
                    move(vVirtualEntry), pProgressDialog, isLaunchFolder);
                extractThread.detach();
            }
            else
            {
                auto vFolderEntry = vector < reference_wrapper<FolderEntry> > { };
                for (const auto &entry : vEntry)
                {
                    vFolderEntry.push_back(
                        dynamic_cast<FolderEntry &>(entry.get()));
                }
                thread extractThread(ExtractFolderEntriesThread,
                    Helper::GetCanonicalPath(tstrPath),
                    move(vFolderEntry), pProgressDialog, isLaunchFolder);
                extractThread.detach();
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
