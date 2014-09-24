#include "stdwx.h"
#include "ArchiveHelper.h"

#include "FileHelper.h"

bool Helper::Extract(TString tstrPath, std::shared_ptr<ArchiveModel> spModel)
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
        auto tstrCanonicalPath = Helper::GetCanonicalPath(path.wstring());
        spModel->Extract(tstrPath);
    }
    catch (const boost::system::system_error &)
    {
        return false;
    }
    return true;
}
