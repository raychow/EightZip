#include "stdwx.h"
#include "SystemImageList.h"

SystemImageList::SystemImageList(bool isSmallIcon)
    : wxImageList()
{
#ifdef __WXMSW__
    auto info = SHFILEINFO{};
    m_hImageList = (WXHIMAGELIST)SHGetFileInfo(wxEmptyString,
        FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_DIRECTORY,
        &info,
        sizeof(SHFILEINFO),
        SHGFI_USEFILEATTRIBUTES | SHGFI_SYSICONINDEX | SHGFI_ICON | (
        isSmallIcon ? SHGFI_SMALLICON : 0));
#endif
}
