// DriveModel.h

#ifndef DRIVEMODEL_H
#define DRIVEMODEL_H

#include "FolderModel.h"

class DriveModel
    : public FolderModel
{
public:
    DriveModel(TString tstrDriveName);

    virtual TString GetItem(ModelItemType itemType) const;

    virtual bool Compare(const ModelBase &otherModel,
        ModelItemType itemType,
        bool isAscending) const;

    virtual bool CanExtract() const;

    virtual ~DriveModel() { }

private:
    wxULongLong_t m_un64TotalSize = 0;
    wxULongLong_t m_un64FreeSpace = 0;

};

#endif // DRIVEMODEL_H
