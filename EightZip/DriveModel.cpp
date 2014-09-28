#include "stdwx.h"
#include "DriveModel.h"

#include "FileInfo.h"
#include "FolderModel.h"

using namespace std;

DriveEntry::DriveEntry(
    TString tstrName,
    wxULongLong_t un64TotalSize,
    wxULongLong_t un64FreeSpace,
    TString tstrType)
{
    m_tstrName = move(tstrName);
    m_un64TotalSize = un64TotalSize;
    m_un64FreeSpace = un64FreeSpace;
    m_tstrType = move(tstrType);
    m_isDirectory = true;
}

std::shared_ptr<IModel> DriveEntry::GetModel()
{
    return make_shared<FolderModel>(GetPath());
}

DriveModel::DriveModel()
{
    m_tstrPath = wxFILE_SEP_PATH;

    for (auto tstrDrive : __GetDrives())
    {
        wxULongLong_t un64TotalNumberOfBytes;
        wxULongLong_t un64TotalNumberOfFreeBytes;

        if (FALSE == ::GetDiskFreeSpaceEx(
            tstrDrive.c_str(),
            nullptr,
            reinterpret_cast<PULARGE_INTEGER>(&un64TotalNumberOfBytes),
            reinterpret_cast<PULARGE_INTEGER>(&un64TotalNumberOfFreeBytes)))
        {
            un64TotalNumberOfBytes = 0;
            un64TotalNumberOfFreeBytes = 0;
        }
        m_vspEntry.push_back(make_shared<DriveEntry>(
            tstrDrive,
            un64TotalNumberOfBytes,
            un64TotalNumberOfFreeBytes,
            FileInfo::GetType(tstrDrive, true, false)));
    }
}

std::shared_ptr<IModel> DriveModel::GetParent() const
{
    return nullptr;
}

const vector<IEntry::ItemType> &DriveModel::GetSupportedItems() const
{
    static vector<IEntry::ItemType> vType = {
        IEntry::ItemType::Name,
        IEntry::ItemType::Type,
        IEntry::ItemType::TotalSize,
        IEntry::ItemType::FreeSpace,
    };
    return vType;
}

vector<TString> DriveModel::__GetDrives()
{
    vector<TString> result;
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
                result.push_back(tstrDrive);
                ptchDrive += tstrDrive.size() + 1;
            }
        }
    }
    return result;
}
