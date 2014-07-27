// FileSystemModel.h

#ifndef FILESYSTEMMODEL_H
#define FILESYSTEMMODEL_H

#include <memory>

#include "ModelBase.h"

class FileSystemModel
    : public ModelBase
{
    struct Private { };

public:
    FileSystemModel(TString tstrFullPath);
    FileSystemModel(Private) { }

    virtual const std::vector<ItemType> &GetChildrenSupportedItems() const;

	virtual bool Compare(const IModel &otherModel, ItemType itemType, bool isAscending) const;

protected:
	virtual void _UpdateTypeInfo() const;
	virtual void _UpdateChildren() const;

private:
    static std::vector<ItemType> m_vType;

    static std::shared_ptr<FileSystemModel> __MakeShared();

	static bool __LocaleCompare(const TString &tstrLeft, const TString & tstrRight, bool isAscending);

};

#endif // FILESYSTEMMODEL_H
