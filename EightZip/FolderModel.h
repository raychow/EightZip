// FolderModel.h

#ifndef FILESYSTEMMODEL_H
#define FILESYSTEMMODEL_H

#include <memory>

#include "ModelBase.h"

class FolderModel
    : public ModelBase
{
public:
    FolderModel(TString tstrPath);
    FolderModel(TString tstrLocation,
        TString tstrFileName,
        wxDateTime dtAccessed,
        wxDateTime dtModified,
        wxDateTime dtCreated);

    virtual std::shared_ptr<ModelBase> GetParent() const;
    virtual const std::vector<ModelItemType> &GetSupportedItems() const;
    virtual bool CanExtract() const;
    virtual const Models GetChildren() const;

    virtual ~FolderModel() { }

private:
    wxDateTime m_dtModified;
    wxDateTime m_dtCreated;
    wxDateTime m_dtAccessed;

};

#endif // FILESYSTEMMODEL_H
