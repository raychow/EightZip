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
    FolderModel(TString tstrLocation, TString tstrName);

    virtual std::shared_ptr<ModelBase> GetParent() const;
    virtual const std::vector<EntryItemType> &GetSupportedItems() const;
    virtual bool IsArchive() const { return false; }

private:
    virtual EntryVector _InitializeEntries() const;

};

#endif // FILESYSTEMMODEL_H
