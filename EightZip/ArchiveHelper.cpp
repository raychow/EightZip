#include "stdwx.h"
#include "ArchiveHelper.h"

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
        ProgressDialog *pProgressDialog)
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
    }

    bool Extract(TString tstrPath, shared_ptr<ArchiveModel> spModel)
    {
        try
        {
            if (!spModel)
            {
                return false;
            }
            shared_ptr<IModel> spFolderModel = spModel;
            while (spFolderModel->IsArchive())
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
                pProgressDialog);
            extractThread.detach();
        }
        catch (const boost::system::system_error &)
        {
            return false;
        }
        return true;
    }
}
