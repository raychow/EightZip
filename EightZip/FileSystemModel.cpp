#include "stdwx.h"
#include "FileSystemModel.h"

#include "FileFinder.h"
#include "FileInfo.h"
#include "FileTypeCache.h"

using namespace std;

#define COMPARE(left, right) isAscending ? (left < right) : (left > right)

FileSystemModel::FileSystemModel(TString tstrFullPath)
{
    FileInfo fileInfo(tstrFullPath);
    if (fileInfo.IsOK())
    {
		while (wxIsPathSeparator(tstrFullPath.back()))
        {
            tstrFullPath.pop_back();
        }
        wxFileName fileName(tstrFullPath);
        fileName.Normalize();
		m_upPrivate->Path = fileName.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
		m_upPrivate->Name = fileName.GetFullName();
		if (m_upPrivate->Name.empty())
		{
			m_upPrivate->Name = move(m_upPrivate->Path);
			while (wxIsPathSeparator(m_upPrivate->Name.back()))
			{
				m_upPrivate->Name.pop_back();
			}
		}
        m_upPrivate->IsDirectory = fileInfo.IsDirectory();
        m_upPrivate->Size = fileInfo.GetSize();
        m_upPrivate->Accessed = fileInfo.GetAccessed();
        m_upPrivate->Modified = fileInfo.GetModified();
        m_upPrivate->Created = fileInfo.GetCreated();
    }
}

const vector<IModel::ItemType> &FileSystemModel::GetChildrenSupportedItems() const
{
    return m_vType;
}

void FileSystemModel::_UpdateTypeInfo() const
{
	lock_guard<mutex> lockGuard(m_upPrivate->UpdateMutex);
	if (-1 == m_upPrivate->IconIndex)
	{
		auto fileTypeInfo = FileTypeCache::GetInfo(IsDirectory(), GetFullPath());
		m_upPrivate->IconIndex = fileTypeInfo.GetIconIndex();
		m_upPrivate->Type = fileTypeInfo.GetName();
	}
}

void FileSystemModel::_UpdateChildren() const
{
    if (!m_upPrivate->IsChildrenValid)
    {
        m_upPrivate->Children.clear();
        if (IsDirectory())
        {
            wxFileName dir(GetPath());
			TString tstrParentFolder = GetFullPath() + wxFILE_SEP_PATH;
            FileFinder finder(tstrParentFolder);

            while (finder.FindNext())
            {
                auto spModel = __MakeShared();
                spModel->m_upPrivate->Path = tstrParentFolder;
                spModel->m_upPrivate->Name = finder.GetFileName();
                spModel->m_upPrivate->IsDirectory = finder.IsDirectory();
                spModel->m_upPrivate->Size = finder.GetSize();
                spModel->m_upPrivate->Accessed = finder.GetAccessed();
                spModel->m_upPrivate->Modified = finder.GetModified();
                spModel->m_upPrivate->Created = finder.GetCreated();
				auto fileTypeInfo = FileTypeCache::GetInfo(spModel->IsDirectory(), spModel->GetFullPath());
				spModel->m_upPrivate->IconIndex = fileTypeInfo.GetIconIndex();
				spModel->m_upPrivate->Type = fileTypeInfo.GetName();
                m_upPrivate->Children.push_back(spModel);
            }
        }
        else
        {
            throw "not implemented yet";
        }
        m_upPrivate->IsChildrenValid = true;
    }
}

std::vector<IModel::ItemType> FileSystemModel::m_vType = {
    IModel::ItemType::Name,
    IModel::ItemType::Size,
    IModel::ItemType::Type,
    IModel::ItemType::Modified,
    IModel::ItemType::Created,
    IModel::ItemType::Accessed,
};

std::shared_ptr<FileSystemModel> FileSystemModel::__MakeShared()
{
    return make_shared<FileSystemModel>(Private());
}

bool FileSystemModel::Compare(const IModel &otherModel, ItemType itemType, bool isAscending) const
{
    const auto &otherFileSystemModel = dynamic_cast<const FileSystemModel &>(otherModel);
	
    switch (itemType)
    {
    case ItemType::Name:
		return __LocaleCompare(m_upPrivate->Name, otherFileSystemModel.m_upPrivate->Name, isAscending);
    case ItemType::Size:
		return COMPARE(m_upPrivate->Size, otherFileSystemModel.m_upPrivate->Size);
    case IModel::ItemType::Type:
		return __LocaleCompare(GetItem(itemType), otherFileSystemModel.GetItem(itemType), isAscending);
    case IModel::ItemType::Modified:
		return COMPARE(m_upPrivate->Modified, otherFileSystemModel.m_upPrivate->Modified);
    case IModel::ItemType::Created:
		return COMPARE(m_upPrivate->Created, otherFileSystemModel.m_upPrivate->Created);
    case IModel::ItemType::Accessed:
		return COMPARE(m_upPrivate->Accessed, otherFileSystemModel.m_upPrivate->Accessed);
    }
    return false;
}

bool FileSystemModel::__LocaleCompare(const TString &tstrLeft, const TString & tstrRight, bool isAscending)
{
	if (tstrLeft.empty() || tstrRight.empty())
	{
		return COMPARE(tstrLeft, tstrRight);
	}

#ifdef __WXMSW__
	switch (::CompareString(GetUserDefaultLCID(), 0, tstrLeft.c_str(), tstrLeft.length(), tstrRight.c_str(), tstrRight.length())) {
	case CSTR_LESS_THAN:
		return isAscending;
	case CSTR_GREATER_THAN:
		return !isAscending;
	default:
		return false;
	}
#endif
}
