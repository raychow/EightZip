#include "stdwx.h"
#include "ModelBase.h"

const TString &ModelBase::GetName() const
{
    _UpdateInfo();
    return m_upPrivate->Name;
}

const TString &ModelBase::GetFullPath() const
{
    _UpdateInfo();
    return m_upPrivate->FullPath;
}

const IModel::ChildVector &ModelBase::GetDirChildren() const
{
    _UpdateChildren();
    return m_upPrivate->DirChildren;
}

const IModel::ChildVector &ModelBase::GetFileChildren() const
{
    _UpdateChildren();
    return m_upPrivate->FileChidren;
}

bool ModelBase::IsFolder() const
{
    _UpdateInfo();
    return m_upPrivate->IsFolder;
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
