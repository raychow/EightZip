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
#include "ProgressDialog.h"
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
            wxTheApp->CallAfter([&](){
                m_pDialog->CenterOnParent();
                m_pDialog->ShowModal();
            });
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

    static void ExtractModelThread(TString tstrExtractPath,
        shared_ptr<VirtualModel> spModel,
        ProgressDialog *pProgressDialog,
        bool isLaunchFolder)
    {
        ProgressDialogManager dialogManager(pProgressDialog);
        ExtractIndicator extractIndicator(pProgressDialog);

        Extractor extractor(tstrExtractPath, &extractIndicator);
        extractor.AddPlan(move(spModel));
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
                OpenFileExternal(tstrExtractPath);
            }
            catch (const SystemException &)
            {
                wxMessageBox(
                    wxString::Format(_("Cannot open \"%s\"."),
                    tstrExtractPath),
                    EIGHTZIP_NAME,
                    wxOK | wxICON_ERROR);
            }
        }
    }

    static void ExtractEntriesThread(TString tstrExtractPath,
        vector<shared_ptr<EntryBase>> vspEntry,
        ProgressDialog *pProgressDialog,
        bool isLaunchFolder)
    {
        ProgressDialogManager dialogManager(pProgressDialog);
        ExtractIndicator extractIndicator(pProgressDialog);

        Extractor extractor(tstrExtractPath, &extractIndicator);
        for (auto &spEntry : vspEntry)
        {
            extractor.AddPlan(move(spEntry));
        }
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
                OpenFileExternal(tstrExtractPath);
            }
            catch (const SystemException &)
            {
                wxMessageBox(
                    wxString::Format(_("Cannot open \"%s\"."),
                    tstrExtractPath),
                    EIGHTZIP_NAME,
                    wxOK | wxICON_ERROR);
            }
        }
    }

    bool Extract(TString tstrPath,
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

            thread extractThread(ExtractModelThread,
                Helper::GetCanonicalPath(tstrPath), spModel,
                pProgressDialog, isLaunchFolder);
            extractThread.detach();
        }
        catch (const boost::system::system_error &)
        {
            return false;
        }
        return true;
    }

    bool Extract(TString tstrPath,
        vector<shared_ptr<EntryBase>> vspEntry,
        bool isLaunchFolder)
    {
        try
        {
            if (vspEntry.empty())
            {
                return false;
            }
            boost::filesystem::create_directories(tstrPath);

            auto *pProgressDialog = new ProgressDialog(
                wxTheApp->GetTopWindow(), wxID_ANY, _("Extracting"));

            thread extractThread(ExtractEntriesThread,
                Helper::GetCanonicalPath(tstrPath), vspEntry,
                pProgressDialog, isLaunchFolder);
            extractThread.detach();
        }
        catch (const boost::system::system_error &)
        {
            return false;
        }
        return true;
    }
}
