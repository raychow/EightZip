#include "stdwx.h"
#include "FileInfo.h"

using namespace std;

std::map<TString, FileInfo> FileInfoManager::m_mInfoCache;

FileInfo FileInfoManager::GetFileInfo(bool isDir, const TString& tstrFileName /*= TString()*/, bool isVirtual /*= false*/)
{
    TString tstrFakeName;
    if (isDir)
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
        , isDir ? FILE_ATTRIBUTE_DIRECTORY : FILE_ATTRIBUTE_NORMAL,
        &info,
        sizeof(SHFILEINFO),
        SHGFI_SYSICONINDEX | SHGFI_TYPENAME | SHGFI_USEFILEATTRIBUTES)))
    {
        auto fileInfo = FileInfo(info.iIcon, info.szTypeName);
        m_mInfoCache[tstrFakeName] = fileInfo;
        return fileInfo;
    }
    return FileInfo();
}
