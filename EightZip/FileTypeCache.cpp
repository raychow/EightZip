#include "stdwx.h"
#include "FileTypeCache.h"

std::map<TString, FileTypeInfo> FileTypeCache::m_mInfoCache;

FileTypeInfo FileTypeCache::GetInfo(bool isDirectory, TString tstrFileName /*= TString()*/)
{
    TString tstrFakeName;
    if (isDirectory)
    {
        tstrFakeName = wxT("fakedir");
    }
    else
    {
        auto szPointPosition = tstrFileName.rfind(wxT('.'));
        tstrFakeName = TString(wxT("fake.")) + tstrFileName.substr(szPointPosition + 1);
    }
    auto iter = m_mInfoCache.find(tstrFakeName);
    if (iter != m_mInfoCache.end())
    {
        return iter->second;
    }

    SHFILEINFO info;
    if (SUCCEEDED(SHGetFileInfo(tstrFakeName.c_str()
        , isDirectory ? FILE_ATTRIBUTE_DIRECTORY : FILE_ATTRIBUTE_NORMAL,
        &info,
        sizeof(SHFILEINFO),
        SHGFI_SYSICONINDEX | SHGFI_TYPENAME | SHGFI_USEFILEATTRIBUTES)))
    {
        auto fileTypeInfo = FileTypeInfo(info.iIcon, info.szTypeName);
        m_mInfoCache[tstrFakeName] = fileTypeInfo;
        return fileTypeInfo;
    }
    return FileTypeInfo();
}
