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
    static void ExtractThread(TString tstrPath,
        shared_ptr<ArchiveModel> spModel,
        ProgressDialog *pProgressDialog)
    {
        wxTheApp->CallAfter([pProgressDialog](){
            pProgressDialog->CenterOnParent();
            pProgressDialog->ShowModal();
        });
        ExtractIndicator extractIndicator(pProgressDialog);

        bool isSuccess = true;
        try
        {
            spModel->Extract(tstrPath, &extractIndicator);
        }
        catch (const SevenZipCore::ArchiveException &)
        {
            isSuccess = false;
        }
        wxTheApp->CallAfter([pProgressDialog, isSuccess](){
            pProgressDialog->Done(isSuccess);
        });
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
            auto path = boost::filesystem::absolute(tstrPath, tstrAbsPath);
            boost::filesystem::create_directories(path);

            auto tstrArchivePath = spModel->GetArchive()->GetPath();
            auto *pProgressDialog = new ProgressDialog(
                wxTheApp->GetTopWindow(), wxID_ANY,
                wxString::Format(_("Extracting from %s"),
                SevenZipCore::Helper::GetFileName(
                tstrArchivePath)));
            pProgressDialog->SetArchivePath(tstrArchivePath);

            thread extractThread(ExtractThread,
                Helper::GetCanonicalPath(path.wstring()), spModel,
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
