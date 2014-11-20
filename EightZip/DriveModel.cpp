#include "stdwx.h"
#include "DriveModel.h"

#ifdef __WXMSW__

#include "DriveEntry.h"

using namespace std;

DriveModel::DriveModel()
    : ModelBase(TString(1, wxFILE_SEP_PATH), wxEmptyString)
{
    
}

const vector<EntryItemType> &DriveModel::GetSupportedItems() const
{
    static vector<EntryItemType> vType = {
        EntryItemType::Name,
        EntryItemType::Type,
        EntryItemType::TotalSize,
        EntryItemType::FreeSpace,
    };
    return vType;
}

DriveModel::EntryVector DriveModel::_InitializeEntries() const
{
    EntryVector result;
    int nBufferLength = ::GetLogicalDriveStrings(0, nullptr);
    if (nBufferLength)
    {
        auto vwchBuffer = vector<wxChar>(nBufferLength);
        if (::GetLogicalDriveStrings(nBufferLength, vwchBuffer.data()))
        {
            const auto *ptchDrive = vwchBuffer.data();
            while (*ptchDrive)
            {
                TString tstrDrive(ptchDrive);
                result.push_back(make_unique<DriveEntry>(tstrDrive));
                ptchDrive += tstrDrive.size() + 1;
            }
        }
    }
    return result;
}

#endif
