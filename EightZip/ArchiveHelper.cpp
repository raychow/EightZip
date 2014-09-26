#include "stdwx.h"
#include "ArchiveHelper.h"

#include <thread>

#include "ExtractIndicator.h"
#include "FileHelper.h"
#include "ProgressDialog.h"

using namespace std;

namespace Helper
{
    static void ExtractThread(TString tstrPath,
        shared_ptr<ArchiveModel> spModel,
        shared_ptr<ProgressDialog> spProgressDialog)
    {
        wxTheApp->CallAfter([spProgressDialog](){
            spProgressDialog->ShowModal();
        });
        ExtractIndicator extractIndicator(spProgressDialog);

        spModel->Extract(tstrPath, &extractIndicator);
        wxTheApp->CallAfter([spProgressDialog](){
            spProgressDialog->Close();
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
            shared_ptr<IModel> spCurrentModel = spModel;
            while (spCurrentModel->IsArchive())
            {
                spCurrentModel = spCurrentModel->GetParent();
            }
            assert(spCurrentModel);
            auto tstrAbsPath = spCurrentModel->GetPath();
            auto path = boost::filesystem::absolute(tstrPath, tstrAbsPath);
            boost::filesystem::create_directories(path);

            auto spProgressDialog = make_shared<ProgressDialog>(
                wxTheApp->GetTopWindow(), wxID_ANY, "");
            thread extractThread(ExtractThread,
                Helper::GetCanonicalPath(path.wstring()), spModel,
                spProgressDialog);
            extractThread.detach();
        }
        catch (const boost::system::system_error &)
        {
            return false;
        }
        return true;
    }
}
