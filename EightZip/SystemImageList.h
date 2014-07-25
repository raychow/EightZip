// SystemImageList.h

#ifndef SYSTEMIMAGELIST_H
#define SYSTEMIMAGELIST_H

#include <map>

#include "SevenZipCore/TString.h"

class SystemImageList
    : public wxImageList
{
public:
    SystemImageList(bool isSmallIcon);

    static int GetIconIndex(bool isDir, const TString& tstrFileName = TString(), bool isVirtual = false);

};

#endif // SYSTEMIMAGELIST_H
