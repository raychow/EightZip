// Helper.h

#ifndef HELPER_H
#define HELPER_H

#include "SevenZipCore/TString.h"

class Helper
{
public:
    static void OpenFileExternal(const TString &tstrPath);
    static wxSizer *AlignBorder(wxSizer *pSizer);
};

#endif // HELPER_H
