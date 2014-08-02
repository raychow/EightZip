#include "stdwx.h"
#include "Helper.h"

#include "Exception.h"

void Helper::OpenFileExternal(TString &tstrPath)
{
#ifdef __WXMSW__
    if ((int)::ShellExecute(
        nullptr, nullptr, tstrPath.c_str(), nullptr, nullptr, SW_SHOWNORMAL
        ) > 32)
    {
        return;
    }
#else
#endif
    throw SystemException("Cannot execute the specified file.");
}
