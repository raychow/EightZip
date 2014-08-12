#include "stdwx.h"
#include "Helper.h"

#include "Exception.h"

void Helper::OpenFileExternal(const TString &tstrPath)
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

wxSizer *Helper::AlignBorder(wxSizer *pSizer)
{
#ifdef __WXMSW__
    int nCount = pSizer->GetItemCount();
    if (nCount)
    {
        auto pSizerLast = pSizer->GetItem(nCount - 1);
        auto nBorderLast = pSizerLast->GetBorder();
        if ((pSizerLast->GetFlag() & wxRIGHT) && SIZER_BORDER > nBorderLast)
        {
            pSizer->AddSpacer(SIZER_BORDER - pSizerLast->GetBorder());
        }
    }
#endif
    return pSizer;
}
