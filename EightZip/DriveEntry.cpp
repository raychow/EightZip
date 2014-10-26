#include "stdwx.h"
#include "DriveEntry.h"

#include "SevenZipCore/CommonHelper.h"

#include "FolderModel.h"

using namespace std;

DriveEntry::DriveEntry(TString tstrName)
    : EntryBase(SevenZipCore::Helper::MakePathSlash(move(tstrName)),
    wxEmptyString, true, false)
{
    if (FALSE == ::GetDiskFreeSpaceEx(
        GetName().c_str(),
        nullptr,
        reinterpret_cast<PULARGE_INTEGER>(&m_un64TotalSize),
        reinterpret_cast<PULARGE_INTEGER>(&m_un64FreeSpace)))
    {
        m_un64TotalSize = 0;
        m_un64FreeSpace = 0;
    }
}

shared_ptr<ModelBase> DriveEntry::GetModel() const
{
    return make_shared<FolderModel>(GetPath());
}

TString DriveEntry::GetItem(EntryItemType itemType) const
{
    switch (itemType)
    {
    case EntryItemType::Name:
        return GetLocation();
    case EntryItemType::TotalSize:
        return ToTString(m_un64TotalSize);
    case EntryItemType::FreeSpace:
        return ToTString(m_un64FreeSpace);
    }
    return EntryBase::GetItem(itemType);
}

bool DriveEntry::Compare(const EntryBase &otherEntry,
    EntryItemType itemType,
    bool isAscending) const
{
    auto &otherDriveEntry = dynamic_cast<const DriveEntry &>(otherEntry);
    switch (itemType)
    {
    case EntryItemType::Name:
        return OrderCompare(
            GetLocation(), otherDriveEntry.GetLocation(), isAscending);
    case EntryItemType::TotalSize:
        return OrderCompare(
            m_un64TotalSize, otherDriveEntry.m_un64TotalSize, isAscending);
    case EntryItemType::FreeSpace:
        return OrderCompare(
            m_un64FreeSpace, otherDriveEntry.m_un64FreeSpace, isAscending);
    }
    return EntryBase::Compare(otherEntry, itemType, isAscending);
}
