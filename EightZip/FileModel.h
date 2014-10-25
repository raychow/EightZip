// FileModel.h

#ifndef FILEMODEL_H
#define FILEMODEL_H

#include "FolderModel.h"

class FileModel
    : public FolderModel
    , public std::enable_shared_from_this<FileModel>
{
public:
    FileModel(TString tstrLocation,
        TString tstrFileName,
        wxDateTime dtAccessed,
        wxDateTime dtModified,
        wxDateTime dtCreated);

    virtual const std::vector<ModelItemType> &GetSupportedItems() const;
    virtual bool CanExtract() const;
    virtual const Models GetChildren() const;

    virtual ~FileModel() { }

private:
    mutable std::shared_ptr<SevenZipCore::Archive> m_spArchive;

};

#endif // FILEMODEL_H
