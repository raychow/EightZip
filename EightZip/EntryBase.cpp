#include "stdwx.h"
#include "EntryBase.h"

#include <array>
#include <algorithm>

#include "SevenZipCore/CommonHelper.h"

#include "FileHelper.h"

using namespace std;

bool LocaleCompare(
    const TString &tstrLeft, const TString &tstrRight, bool isAscending)
{
    if (tstrLeft.empty() || tstrRight.empty())
    {
        return OrderCompare(tstrLeft, tstrRight, isAscending);
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

void EntryBase::OpenExternal() const
{
    Helper::OpenFileExternal(GetPath());
}

TString EntryBase::GetItem(EntryItemType itemType) const
{
    switch (itemType)
    {
    case EntryItemType::Name:
        return GetName();
    case EntryItemType::Type:
        return GetType();
    }
    return wxEmptyString;
}

bool EntryBase::Compare(const EntryBase &otherModel,
    EntryItemType itemType,
    bool isAscending) const
{
    switch (itemType)
    {
    case EntryItemType::Name:
        return LocaleCompare(
            GetName(), otherModel.GetName(), isAscending);
    case EntryItemType::Type:
        return LocaleCompare(
            GetType(), otherModel.GetType(), isAscending);
    }
    return false;
}

EntryBase::EntryBase(TString tstrLocation,
    TString tstrName,
    TString tstrType,
    bool isDirectory,
    bool isVirtual)
    : m_tstrLocation(SevenZipCore::Helper::MakePathSlash(move(tstrLocation)))
    , m_tstrName(move(tstrName))
    , m_tstrType(move(tstrType))
    , m_isDirectory(isDirectory)
    , m_isVirtual(isVirtual)
{

}

EntryBase::EntryBase(TString tstrLocation,
    TString tstrName,
    bool isDirectory,
    bool isVirtual)
    : m_tstrLocation(SevenZipCore::Helper::MakePathSlash(move(tstrLocation)))
    , m_tstrName(move(tstrName))
    , m_tstrType(FileInfo::GetType(GetName(), isDirectory, isVirtual))
    , m_isDirectory(isDirectory)
    , m_isVirtual(isVirtual)
{

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
    if (IsDirectory())
    {
        return false;
    }
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
