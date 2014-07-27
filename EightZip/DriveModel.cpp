#include "stdwx.h"
#include "DriveModel.h"

#include "FileSystemModel.h"

using namespace std;

std::shared_ptr<IModel> DriveModel::GetParent() const
{
    return nullptr;
}

const vector<IModel::ItemType> &DriveModel::GetChildrenSupportedItems() const
{
    return m_vType;
}

void DriveModel::_UpdateChildren()
{
    if (!m_isChildrenValid)
    {
        m_vspChild.clear();
#ifdef __WXMSW__
        for (auto tstrDrive : __GetDrives())
        {
            m_vspChild.push_back(make_shared<FileSystemModel>(tstrDrive));
        }
#endif
        m_isChildrenValid = true;
    }
}

std::vector<IModel::ItemType> DriveModel::m_vType = {
    IModel::ItemType::Name,
    IModel::ItemType::Type,
    IModel::ItemType::TotalSize,
    IModel::ItemType::FreeSpace,
};

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