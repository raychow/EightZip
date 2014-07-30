#include "stdwx.h"
#include "ModelBase.h"

#include "FileInfo.h"

using namespace std;

int EntryBase::GetIconIndex() const
{
    if (-1 == m_nIconIndex)
    {
        m_nIconIndex = FileInfo::GetIconIndex(GetFullPath(), IsDirectory(), false);
    }
    return m_nIconIndex;
}

TString EntryBase::GetItem(ItemType itemType) const
{
    try
    {
        switch (itemType)
        {
        case ItemType::Name:
            return m_tstrName;
        case ItemType::Size:
            if (!IsDirectory())
            {
                return ToTString(m_un64Size);
            }
        case ItemType::PackedSize:
            if (!IsDirectory())
            {
                return ToTString(m_un64PackedSize);
            }
        case IEntry::ItemType::TotalSize:
            return ToTString(m_un64TotalSize);
        case IEntry::ItemType::FreeSpace:
            return ToTString(m_un64FreeSpace);
        case ItemType::Type:
            return m_tstrType;
        case ItemType::Modified:
            return m_dtModified.FormatISOCombined(' ').ToStdWstring();
        case ItemType::Created:
            return m_dtCreated.FormatISOCombined(' ').ToStdWstring();
        case ItemType::Accessed:
            return m_dtAccessed.FormatISOCombined(' ').ToStdWstring();
        case ItemType::Attributes:
            break;
        case ItemType::CRC:
            if (m_oun32CRC)
            {
                TStringStream tss;
                tss << hex << setw(8) << setfill(wxT('0')) << uppercase << *m_oun32CRC;
                return tss.str();
            }
        default:
            break;
        }
    }
    catch (std::exception)
    {
    }
    return wxEmptyString;
}

bool EntryBase::Compare(const IEntry &otherEntry, ItemType itemType, bool isAscending) const
{
    const auto &otherEntryBase = dynamic_cast<const EntryBase &>(otherEntry);

    switch (itemType)
    {
    case ItemType::Name:
        return _LocaleCompare(m_tstrName, otherEntryBase.m_tstrName, isAscending);
    case ItemType::Size:
        return COMPARE(m_un64Size, otherEntryBase.m_un64Size, isAscending);
    case ItemType::PackedSize:
        return COMPARE(m_un64PackedSize, otherEntryBase.m_un64PackedSize, isAscending);
    case ItemType::Type:
        return _LocaleCompare(GetItem(itemType), otherEntryBase.GetItem(itemType), isAscending);
    case ItemType::Modified:
        return COMPARE(m_dtModified, otherEntryBase.m_dtModified, isAscending);
    case ItemType::Created:
        return COMPARE(m_dtCreated, otherEntryBase.m_dtCreated, isAscending);
    case ItemType::Accessed:
        return COMPARE(m_dtAccessed, otherEntryBase.m_dtAccessed, isAscending);
    }
    return false;
}

bool EntryBase::_LocaleCompare(const TString &tstrLeft, const TString & tstrRight, bool isAscending)
{
    if (tstrLeft.empty() || tstrRight.empty())
    {
        return COMPARE(tstrLeft, tstrRight, isAscending);
    }

#ifdef __WXMSW__
    switch (::CompareString(GetUserDefaultLCID(), 0, tstrLeft.c_str(), tstrLeft.size(), tstrRight.c_str(), tstrRight.size())) {
    case CSTR_LESS_THAN:
        return isAscending;
    case CSTR_GREATER_THAN:
        return !isAscending;
    default:
        return false;
    }
#endif
}