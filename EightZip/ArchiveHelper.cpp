#include "stdwx.h"
#include "ArchiveHelper.h"

#include <set>
#include <thread>

#include "SevenZipCore/CommonHelper.h"
#include "SevenZipCore/Exception.h"

#include "ExtractIndicator.h"
#include "FileHelper.h"
#include "ProgressDialog.h"

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

    static void ExtractThread(TString tstrExtractPath,
        shared_ptr<ArchiveModel> spModel,
        ProgressDialog *pProgressDialog,
        bool isLaunchFolder)
    {
        ProgressDialogManager dialogManager(pProgressDialog);
        ExtractIndicator extractIndicator(pProgressDialog);
        try
        {
            spModel->Extract(tstrExtractPath, &extractIndicator);
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

    //static void ExtractThread(TString tstrExtractPath,
    //    shared_ptr<ArchiveModel> spModel,
    //    vector<shared_ptr<ArchiveEntry>> vspEntry,
    //    ProgressDialog *pProgressDialog,
    //    bool isLaunchFolder)
    //{
    //    ProgressDialogManager dialogManager(pProgressDialog);
    //    ExtractIndicator extractIndicator(pProgressDialog);

    //    try
    //    {
    //        set<UINT32> snIndex;
    //        for (const auto &spEntry : vspEntry)
    //        {
    //            auto entryIndexes = spEntry->GetAllArchiveIndexes();
    //            snIndex.insert(entryIndexes.cbegin(), entryIndexes.cend());
    //        }
    //        spModel->Extract(vector<UINT32>(snIndex.cbegin(), snIndex.cend()),
    //            tstrExtractPath, &extractIndicator);
    //    }
    //    catch (const SevenZipCore::ArchiveException &)
    //    {
    //        return;
    //    }
    //    dialogManager.SetSuccess(true);
    //    if (isLaunchFolder)
    //    {
    //        try
    //        {
    //            OpenFileExternal(tstrExtractPath);
    //        }
    //        catch (const SystemException &)
    //        {
    //            wxMessageBox(
    //                wxString::Format(_("Cannot open \"%s\"."),
    //                tstrExtractPath),
    //                EIGHTZIP_NAME,
    //                wxOK | wxICON_ERROR);
    //        }
    //    }
    //}

    bool Extract(TString tstrPath,
        shared_ptr<ArchiveModel> spModel,
        bool isLaunchFolder)
    {
        if (!spModel)
        {
            return false;
        }
        try
        {
            shared_ptr<IModel> spFolderModel = spModel;
            while (spFolderModel->IsVirtual())
            {
                spFolderModel = spFolderModel->GetParent();
            }
            assert(spFolderModel);
            auto tstrAbsPath = spFolderModel->GetPath();
            auto extractPath = boost::filesystem::absolute(
                tstrPath, tstrAbsPath);
            boost::filesystem::create_directories(extractPath);

            auto tstrArchivePath = spModel->GetArchive()->GetPath();
            auto *pProgressDialog = new ProgressDialog(
                wxTheApp->GetTopWindow(), wxID_ANY,
                wxString::Format(_("Extracting from %s"),
                SevenZipCore::Helper::GetFileName(
                tstrArchivePath)));
            pProgressDialog->SetArchivePath(tstrArchivePath);

            thread extractThread(ExtractThread,
                Helper::GetCanonicalPath(extractPath.wstring()), spModel,
                pProgressDialog, isLaunchFolder);
            extractThread.detach();
        }
        catch (const boost::system::system_error &)
        {
            return false;
        }
        return true;
    }

    //bool Extract(TString tstrPath,
    //    // vector<spArchiveModel, vector<spArchiveEntry>>
    //    shared_ptr<ArchiveModel> spModel,
    //    vector<shared_ptr<ArchiveEntry>> vspEntry,
    //    bool isLaunchFolder)
    //{
    //    if (!spModel || vspEntry.empty())
    //    {
    //        return false;
    //    }
    //    try
    //    {
    //        shared_ptr<IModel> spFolderModel = spModel;
    //        while (spFolderModel->IsArchive())
    //        {
    //            spFolderModel = spFolderModel->GetParent();
    //        }
    //        assert(spFolderModel);
    //        auto tstrAbsPath = spFolderModel->GetPath();
    //        auto extractPath = boost::filesystem::absolute(
    //            tstrPath, tstrAbsPath);
    //        boost::filesystem::create_directories(extractPath);

    //        auto tstrArchivePath = spModel->GetArchive()->GetPath();
    //        auto *pProgressDialog = new ProgressDialog(
    //            wxTheApp->GetTopWindow(), wxID_ANY,
    //            wxString::Format(_("Extracting from %s"),
    //            SevenZipCore::Helper::GetFileName(
    //            tstrArchivePath)));
    //        pProgressDialog->SetArchivePath(tstrArchivePath);

    //        thread extractThread(ExtractThread,
    //            Helper::GetCanonicalPath(extractPath.wstring()), move(vspEntry),
    //            pProgressDialog, isLaunchFolder);
    //        extractThread.detach();
    //    }
    //    catch (const boost::system::system_error &)
    //    {
    //        return false;
    //    }
    //    return true;
    //}
}
