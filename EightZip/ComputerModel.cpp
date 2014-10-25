#include "stdwx.h"
#include "ComputerModel.h"

#include "FileInfo.h"
#include "DriveModel.h"

using namespace std;

ComputerModel::ComputerModel()
    : ModelBase(TString(1, wxFILE_SEP_PATH),
    wxEmptyString,
    _("Computer").ToStdWstring(),
    true,
    false)
{

}

shared_ptr<ModelBase> ComputerModel::GetParent() const
{
    return nullptr;
}

const vector<ModelItemType> &ComputerModel::GetSupportedItems() const
{
    static vector<ModelItemType> vType = {
        ModelItemType::Name,
        ModelItemType::Type,
        ModelItemType::TotalSize,
        ModelItemType::FreeSpace,
    };
    return vType;
}

bool ComputerModel::CanExtract() const
{
    return false;
}

const ModelBase::Models ComputerModel::GetChildren() const
{
    Models result;
    int nBufferLength = ::GetLogicalDriveStrings(0, nullptr);
    if (nBufferLength)
    {
        std::unique_ptr<wxChar[]> uptchBuffer(new wxChar[nBufferLength]);
        if (::GetLogicalDriveStrings(nBufferLength, uptchBuffer.get()))
        {
            const auto *ptchDrive = uptchBuffer.get();
            while (*ptchDrive)
            {
                TString tstrDrive(ptchDrive);
                result.push_back(make_shared<DriveModel>(tstrDrive));
                ptchDrive += tstrDrive.size() + 1;
            }
        }
    }
    return result;
}
