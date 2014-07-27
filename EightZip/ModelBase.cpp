#include "stdwx.h"
#include "ModelBase.h"

#include "FileInfo.h"

using namespace std;

int ModelBase::GetIconIndex() const
{
    if (-1 == m_nIconIndex)
    {
        m_nIconIndex = FileInfo::GetIconIndex(GetFullPath(), IsDirectory(), false);
    }
    return m_nIconIndex;
}

TString ModelBase::GetItem(ItemType itemType) const
{
    switch (itemType)
    {
    case IModel::ItemType::Name:
        return m_tstrName;
    case IModel::ItemType::Size:
        return IsDirectory() ? wxT("") : ToTString(m_un64Size);
    case IModel::ItemType::Type:
        return m_tstrType;
    case IModel::ItemType::Modified:
        return m_dtModified.FormatISOCombined(' ').ToStdWstring();
    case IModel::ItemType::Created:
        return m_dtCreated.FormatISOCombined(' ').ToStdWstring();
    case IModel::ItemType::Accessed:
        return m_dtAccessed.FormatISOCombined(' ').ToStdWstring();
    case IModel::ItemType::Attributes:
        break;
    default:
        break;
    }
    return wxT("");
}

bool ModelBase::Compare(const IModel &otherModel, ItemType itemType, bool isAscending) const
{
    const auto &otherModelBase = dynamic_cast<const ModelBase &>(otherModel);

    switch (itemType)
    {
    case ItemType::Name:
        return _LocaleCompare(m_tstrName, otherModelBase.m_tstrName, isAscending);
    case ItemType::Size:
        return COMPARE(m_un64Size, otherModelBase.m_un64Size, isAscending);
    case IModel::ItemType::Type:
        return _LocaleCompare(GetItem(itemType), otherModelBase.GetItem(itemType), isAscending);
    case IModel::ItemType::Modified:
        return COMPARE(m_dtModified, otherModelBase.m_dtModified, isAscending);
    case IModel::ItemType::Created:
        return COMPARE(m_dtCreated, otherModelBase.m_dtCreated, isAscending);
    case IModel::ItemType::Accessed:
        return COMPARE(m_dtAccessed, otherModelBase.m_dtAccessed, isAscending);
    }
    return false;
}

bool ModelBase::_LocaleCompare(const TString &tstrLeft, const TString & tstrRight, bool isAscending)
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