#include "stdwx.h"
#include "SystemImageList.h"

SystemImageList::SystemImageList(bool isSmallIcon)
    : wxImageList()
{
    SHFILEINFO info;
    m_hImageList = (WXHIMAGELIST)SHGetFileInfo(wxT("")
        , FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_DIRECTORY
        , &info
        , sizeof(SHFILEINFO)
        , SHGFI_USEFILEATTRIBUTES | SHGFI_SYSICONINDEX | SHGFI_ICON | (isSmallIcon ? SHGFI_SMALLICON : 0));
}

int SystemImageList::GetIconIndex(bool isDir, const TString& tstrFileName /*= TString()*/, bool isVirtual /*= false*/)
{
    SHFILEINFO info;
    if (SUCCEEDED(SHGetFileInfo(tstrFileName.c_str()
        , isDir ? FILE_ATTRIBUTE_DIRECTORY : FILE_ATTRIBUTE_NORMAL,
        &info,
        sizeof(SHFILEINFO),
        SHGFI_ICON | (isVirtual ? SHGFI_USEFILEATTRIBUTES : 0))))
    {
        int result = info.iIcon;
        DestroyIcon(info.hIcon);
        return result;
    }
    return -1;
}
