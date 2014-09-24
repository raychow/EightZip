#include "stdwx.h"
#include "ModelBase.h"

#include "SevenZipCore/CommonHelper.h"
#include "SevenZipCore/ComPtr.h"
#include "SevenZipCore/OpenCallback.h"
#include "SevenZipCore/Platform.h"

#include "ArchiveModel.h"
#include "DriveModel.h"
#include "FileHelper.h"
#include "FileInfo.h"
#include "FolderModel.h"

using namespace std;

int EntryBase::GetIconIndex() const
{
    if (-1 == m_nIconIndex)
    {
        m_nIconIndex = FileInfo::GetIconIndex(
            GetPath(), IsDirectory(), false);
    }
    return m_nIconIndex;
}

TString EntryBase::GetItem(ItemType itemType) const
{
    try
    {
        switch (itemType)
        {
        case ItemType::Name:
            return m_tstrName;
        case ItemType::Size:
            if (!IsDirectory())
            {
                return ToTString(m_un64Size);
            }
        case ItemType::PackedSize:
            if (!IsDirectory())
            {
                return ToTString(m_un64PackedSize);
            }
        case IEntry::ItemType::TotalSize:
            return ToTString(m_un64TotalSize);
        case IEntry::ItemType::FreeSpace:
            return ToTString(m_un64FreeSpace);
        case ItemType::Type:
            return m_tstrType;
        case ItemType::Modified:
            return m_dtModified.FormatISOCombined(' ').ToStdWstring();
        case ItemType::Created:
            return m_dtCreated.FormatISOCombined(' ').ToStdWstring();
        case ItemType::Accessed:
            return m_dtAccessed.FormatISOCombined(' ').ToStdWstring();
        case ItemType::Attributes:
            break;
        case ItemType::CRC:
            if (m_oun32CRC)
            {
                TStringStream tss;
                tss << hex << setw(8) << setfill(wxT('0'));
                tss << uppercase << *m_oun32CRC;
                return tss.str();
            }
        default:
            break;
        }
    }
    catch (const exception &)
    {
    }
    return wxEmptyString;
}

bool EntryBase::IsOpenExternal() const
{
#ifdef __WXMSW__
    static array<TString, 44> atstrDirectExtension = {
        wxT("bat"),
        wxT("chm"),
        wxT("com"),
        wxT("doc"),
        wxT("docm"),
        wxT("docx"),
        wxT("dotm"),
        wxT("dotx"),
        wxT("dwf"),
        wxT("exe"),
        wxT("flv"),
        wxT("mpp"),
        wxT("msg"),
        wxT("msi"),
        wxT("ods"),
        wxT("odt"),
        wxT("pdf"),
        wxT("potm"),
        wxT("potx"),
        wxT("ppam"),
        wxT("pps"),
        wxT("ppsm"),
        wxT("ppsx"),
        wxT("ppt"),
        wxT("pptm"),
        wxT("pptx"),
        wxT("pub"),
        wxT("swf"),
        wxT("vsd"),
        wxT("wb3"),
        wxT("wdb"),
        wxT("wks"),
        wxT("wps"),
        wxT("wpt"),
        wxT("xlam"),
        wxT("xlr"),
        wxT("xls"),
        wxT("xlsb"),
        wxT("xlsm"),
        wxT("xlsx"),
        wxT("xltm"),
        wxT("xltx"),
        wxT("xps"),
        wxT("xsn")
    };
#else
#endif
    auto szPointLocation = m_tstrName.rfind(wxFILE_SEP_EXT);
    if (m_tstrName.npos == szPointLocation)
    {
        return false;
    }
    auto tstrExtension = m_tstrName.substr(szPointLocation + 1);
    return binary_search(atstrDirectExtension.cbegin(),
        atstrDirectExtension.cend(),
        tstrExtension);
}

void EntryBase::OpenExternal()
{
    Helper::OpenFileExternal(GetPath());
}

bool EntryBase::Compare(
    const IEntry &otherEntry, ItemType itemType, bool isAscending) const
{
    const auto &otherEntryBase = dynamic_cast<const EntryBase &>(otherEntry);

    switch (itemType)
    {
    case ItemType::Name:
        return _LocaleCompare(
            m_tstrName, otherEntryBase.m_tstrName, isAscending);
    case ItemType::Size:
        return COMPARE(m_un64Size, otherEntryBase.m_un64Size, isAscending);
    case ItemType::PackedSize:
        return COMPARE(
            m_un64PackedSize, otherEntryBase.m_un64PackedSize, isAscending);
    case ItemType::Type:
        return _LocaleCompare(
            GetItem(itemType), otherEntryBase.GetItem(itemType), isAscending);
    case ItemType::Modified:
        return COMPARE(m_dtModified, otherEntryBase.m_dtModified, isAscending);
    case ItemType::Created:
        return COMPARE(m_dtCreated, otherEntryBase.m_dtCreated, isAscending);
    case ItemType::Accessed:
        return COMPARE(m_dtAccessed, otherEntryBase.m_dtAccessed, isAscending);
    }
    return false;
}

bool EntryBase::_LocaleCompare(
    const TString &tstrLeft, const TString & tstrRight, bool isAscending)
{
    if (tstrLeft.empty() || tstrRight.empty())
    {
        return COMPARE(tstrLeft, tstrRight, isAscending);
    }

#ifdef __WXMSW__
    switch (::CompareString(
        GetUserDefaultLCID(),
        0, tstrLeft.c_str(),
        tstrLeft.size(),
        tstrRight.c_str(),
        tstrRight.size())) {
    case CSTR_LESS_THAN:
        return isAscending;
    case CSTR_GREATER_THAN:
        return !isAscending;
    default:
        return false;
    }
#endif
}

TString ModelBase::GetName() const
{
    if (m_tstrPath.back() == wxFILE_SEP_PATH)
    {
        return m_tstrPath;
    }
    return m_tstrPath.substr(m_tstrPath.rfind(wxFILE_SEP_PATH) + 1);
}

bool ModelBase::HasParent() const
{
    auto &tstrPath = GetPath();
    return !tstrPath.empty() &&
        tstrPath.size() != 1 || TString::npos == tstrPath.find_first_of(
        FOLDER_POSSIBLE_SEPARATOR);
}

shared_ptr<IModel> GetModelFromPath(
    TString tstrPath, bool isTryOpenArchive/* = true*/)
{
    if (tstrPath.size() != 1 || TString::npos == tstrPath.find_first_of(
        FOLDER_POSSIBLE_SEPARATOR))
    {
        TString tstrOriginalPath = tstrPath;
        tstrPath = Helper::GetCanonicalPath(move(tstrPath));
        while (!tstrPath.empty())
        {
            int attributes = Helper::GetFileAttributes(tstrPath);
            if (attributes & EIGHT_FILE_STATUS_DIRECTORY)
            {
                return make_shared<FolderModel>(tstrPath);
            }
            else if (isTryOpenArchive && attributes & EIGHT_FILE_STATUS_FILE)
            {
                auto spModel = make_shared<ArchiveModel>(
                    nullptr,
                    tstrPath,
                    wxEmptyString,
                    tstrPath,
                    SevenZipCore::MakeComPtr(new SevenZipCore::OpenCallback));
                spModel->LoadChildren();
                return GetModelFromPath(spModel, tstrOriginalPath);
            }
            tstrPath = SevenZipCore::Helper::RemovePathSlash(move(tstrPath));
            auto szLocation = tstrPath.find_last_of(FOLDER_POSSIBLE_SEPARATOR);
            if (TString::npos == szLocation)
            {
                break;
            }
            tstrPath = tstrPath.substr(0, szLocation);
        }
    }
#ifdef __WXMSW__
    return make_shared<DriveModel>();
#else
    return make_shared<FolderModel>(wxT("/"));
#endif
}

shared_ptr<IModel> GetModelFromPath(
    shared_ptr<IModel> spModel, TString tstrPath)
{
    if (!spModel || !spModel->IsArchive())
    {
        spModel = GetModelFromPath(tstrPath);
    }
    if (!spModel->IsArchive())
    {
        return spModel;
    }
    tstrPath = Helper::GetCanonicalPath(move(tstrPath));
    auto vtstrPathPart = SevenZipCore::Helper::SplitString(
        tstrPath, FOLDER_POSSIBLE_SEPARATOR, true);
    auto vtstrModelPathPart = SevenZipCore::Helper::SplitString(
        spModel->GetPath(), FOLDER_POSSIBLE_SEPARATOR, true);
    auto szMinSize = min(vtstrPathPart.size(), vtstrModelPathPart.size());
    int i = 0;
    while (i != szMinSize && Helper::IsPathEqual(
        vtstrPathPart[i], vtstrModelPathPart[i]))
    {
        ++i;
    }
    int nDiffCount = vtstrModelPathPart.size() - i;
    while (nDiffCount--)
    {
        spModel = spModel->GetParent();
        if (!spModel->IsArchive())
        {
            return GetModelFromPath(tstrPath);
        }
    }
    while (i != vtstrPathPart.size())
    {
        auto entries = spModel->GetEntries();
        auto iter = find_if(entries.cbegin(), entries.cend(),
            [&vtstrPathPart, i](const shared_ptr<IEntry> &spEntry) {
            return spEntry->GetName() == vtstrPathPart[i];
        });
        if (entries.cend() == iter || !(*iter)->IsDirectory())
        {
            break;
        }
        else
        {
            spModel = (*iter)->GetModel();
        }
        ++i;
    }
    return spModel;
}
