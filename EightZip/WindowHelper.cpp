#include "stdwx.h"
#include "WindowHelper.h"

#include <algorithm>
#include <memory>

#include "SevenZipCore/CommonHelper.h"

#include "Exception.h"

using namespace std;

namespace Helper
{
    wxSizer *AlignBorder(wxSizer *pSizer)
    {
#ifdef __WXMSW__
        int nCount = pSizer->GetItemCount();
        if (nCount)
        {
            auto *pSizerLast = pSizer->GetItem(nCount - 1);
            auto nBorderLast = pSizerLast->GetBorder();
            if ((pSizerLast->GetFlag() & wxRIGHT) && SIZER_BORDER > nBorderLast)
            {
                pSizer->AddSpacer(SIZER_BORDER - pSizerLast->GetBorder());
            }
        }
#endif
        return pSizer;
    }
}

