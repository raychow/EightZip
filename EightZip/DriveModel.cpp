#include "stdwx.h"
#include "DriveModel.h"

#include "FileInfo.h"
#include "FileSystemModel.h"

using namespace std;

std::vector<IEntry::ItemType> DriveModel::m_vType = {
    IEntry::ItemType::Name,
    IEntry::ItemType::Type,
    IEntry::ItemType::TotalSize,
    IEntry::ItemType::FreeSpace,
};

DriveEntry::DriveEntry(TString tstrName, wxULongLong_t un64TotalSize, wxULongLong_t un64FreeSpace, TString tstrType)
{
    m_tstrName = tstrName;
    m_un64TotalSize = un64TotalSize;
    m_un64FreeSpace = un64FreeSpace;
    m_tstrType = tstrType;
}

std::shared_ptr<IModel> DriveEntry::GetModel() const
{
    return make_shared<FileSystemModel>(GetFullPath());
}

DriveModel::DriveModel()
{
#ifdef __WXMSW__
    for (auto tstrDrive : __GetDrives())
    {
        wxULongLong_t un64TotalNumberOfBytes;
        wxULongLong_t un64TotalNumberOfFreeBytes;

        if (FALSE == ::GetDiskFreeSpaceEx(tstrDrive.c_str(), nullptr, reinterpret_cast<PULARGE_INTEGER>(&un64TotalNumberOfBytes), reinterpret_cast<PULARGE_INTEGER>(&un64TotalNumberOfFreeBytes)))
        {
            un64TotalNumberOfBytes = 0;
            un64TotalNumberOfFreeBytes = 0;
        }
        m_vspEntry.push_back(make_shared<DriveEntry>(tstrDrive, un64TotalNumberOfBytes, un64TotalNumberOfFreeBytes, FileInfo::GetType(tstrDrive, true, false)));
    }
#endif
}

std::shared_ptr<IModel> DriveModel::GetParent() const
{
    return nullptr;
}

const vector<IEntry::ItemType> &DriveModel::GetSupportedItems() const
{
    return m_vType;
}

#ifdef __WXMSW__
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
#endif