#include "stdwx.h"
#include "ModelBase.h"

using namespace std;

const TString & ModelBase::GetName() const
{
    return m_upPrivate->Name;
}

const TString &ModelBase::GetPath() const
{
    return m_upPrivate->Path;
}

TString ModelBase::GetFullPath() const
{
    return GetPath() + GetName();
}

const IModel::ChildVector &ModelBase::GetChildren() const
{
    _UpdateChildren();
    return m_upPrivate->Children;
}

int ModelBase::GetIconIndex() const
{
	return m_upPrivate->IconIndex;
}

TString ModelBase::GetItem(ItemType itemType) const
{
	switch (itemType)
	{
	case IModel::ItemType::Name:
		return m_upPrivate->Name;
	case IModel::ItemType::Size:
		return IsDirectory() ? wxT("") : ToTString(m_upPrivate->Size);
	case IModel::ItemType::Type:
		return m_upPrivate->Type;
	case IModel::ItemType::Modified:
		return m_upPrivate->Modified.FormatISOCombined(' ').ToStdWstring();
	case IModel::ItemType::Created:
		return m_upPrivate->Created.FormatISOCombined(' ').ToStdWstring();
	case IModel::ItemType::Accessed:
		return m_upPrivate->Accessed.FormatISOCombined(' ').ToStdWstring();
	case IModel::ItemType::Attributes:
		break;
	default:
		break;
	}
	return wxT("");
}

bool ModelBase::IsDirectory() const
{
    return m_upPrivate->IsDirectory;
}

bool ModelBase::IsOpenInside() const
{
    return m_upPrivate->IsOpenInside;
}

void ModelBase::Invalid() const
{
    m_upPrivate->IsValid = false;
    m_upPrivate->IsChildrenValid = false;
}
