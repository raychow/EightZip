#include "stdwx.h"
#include "DriveModel.h"

#include "SevenZipCore/CommonHelper.h"

DriveModel::DriveModel(TString tstrDriveName)
    : FolderModel(SevenZipCore::Helper::MakePathSlash(tstrDriveName))
{
    if (FALSE == ::GetDiskFreeSpaceEx(
        tstrDriveName.c_str(),
        nullptr,
        reinterpret_cast<PULARGE_INTEGER>(&m_un64TotalSize),
        reinterpret_cast<PULARGE_INTEGER>(&m_un64FreeSpace)))
    {
        m_un64TotalSize = 0;
        m_un64FreeSpace = 0;
    }
}

TString DriveModel::GetItem(ModelItemType itemType) const
{
    switch (itemType)
    {
    case ModelItemType::TotalSize:
        return ToTString(m_un64TotalSize);
    case ModelItemType::FreeSpace:
        return ToTString(m_un64FreeSpace);
    }
    return FolderModel::GetItem(itemType);
}

bool DriveModel::Compare(const ModelBase &otherModel,
    ModelItemType itemType,
    bool isAscending) const
{
    auto &otherDriveModel = dynamic_cast<const DriveModel &>(otherModel);
    switch (itemType)
    {
    case ModelItemType::TotalSize:
        return OrderCompare(
            m_un64TotalSize, otherDriveModel.m_un64TotalSize, isAscending);
    case ModelItemType::FreeSpace:
        return OrderCompare(
            m_un64FreeSpace, otherDriveModel.m_un64FreeSpace, isAscending);
    }
    return FolderModel::Compare(otherModel, itemType, isAscending);
}

bool DriveModel::CanExtract() const
{
    return false;
}
