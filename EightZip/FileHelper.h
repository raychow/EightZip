// FileHelper.h

#ifndef FILEHELPER_H
#define FILEHELPER_H

#include "SevenZipCore/TString.h"

enum FileAttributes
{
    EIGHT_FILE_STATUS_DIRECTORY = 0x01,
    EIGHT_FILE_STATUS_FILE = 0x02,
    EIGHT_FILE_STATUS_SYMBOLIC_LINK = 0x04,
};

namespace Helper
{
    void OpenFileExternal(const TString &tstrPath);
    int GetFileAttributes(TString tstrPath);
    TString GetCanonicalPath(TString tstrPath);
    int GetCommonPrefixLength(const TString &tstr1,
        const TString &tstr2,
        bool isCaseSensitive = false);
    bool IsPathEqual(const TString &tstrLeft, const TString &tstrRight);
    TString GetLocation(const TString &tstrPath);

}

#endif // FILEHELPER_H
