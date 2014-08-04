// Common.h

#ifndef SEVENZIPCORE_COMMON_H
#define SEVENZIPCORE_COMMON_H

#include <vector>

#include <boost/optional.hpp>

#include <Windows.h>

#include "Platform.h"
#include "TString.h"

namespace SevenZipCore
{
    class PropertyVariant;

    class Helper
    {
    public:
        static TString GetFileName(const TString &tstrPath);
        static TString GetFileNameStem(const TString &tstrPath);
        static TString GetFileExtension(const TString &tstrExtension);

        static TString ToUpper(const TString &value);
        static TString ToLower(const TString &value);

        static std::vector<TString> SplitString(
            const TString &tstrPath,
            const TString &tstrSeparators,
            bool isSkipEmptyPart = false);
        static std::vector<TString> GetFilteredPath(
            std::vector<TString> vtstrPathPart);
        static TString GetFilteredFileName(TString tstrPathPart);
        static TString JoinString(
            std::vector<TString> vtstrPathPart,
            const TString &tstrSeparators,
            bool isSkipEmptyPart = false);
        static void SetFileTime(
            TString tstrPath,
            const FILETIME *lpCreationTime,
            const FILETIME *lpLastAccessTime,
            const FILETIME *lpLastWriteTime);
        static boost::optional<FILETIME> GetFileModifiedTime(TString tstrPath);
        static bool AutoRenamePath(TString &tstrPath);
    };
}

#endif // SEVENZIPCORE_COMMON_H