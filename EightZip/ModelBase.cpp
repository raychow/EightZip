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

bool ModelBase::IsDirectory() const
{
    return m_upPrivate->IsDirectory;
}

bool ModelBase::IsOpenInside() const
{
    _UpdateInfo();
    return m_upPrivate->IsOpenInside;
}

void ModelBase::Invalid() const
{
    m_upPrivate->IsValid = false;
    m_upPrivate->IsChildrenValid = false;
}