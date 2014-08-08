// CommonHelper.h

#ifndef SEVENZIPCORE_COMMONHELPER_H
#define SEVENZIPCORE_COMMONHELPER_H

#include <memory>
#include <vector>

#include <boost/optional.hpp>

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

        static TString MakePathSlash(TString value);
        static TString RemovePathSlash(TString value);
        static bool IsEndOfSlash(const TString &value);
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
        static time_t GetUnixTimeFromFileTime(const FILETIME &fileTime);

        template<typename T, typename U>
        static std::shared_ptr<T> QueryInterface(U p, REFGUID iid)
        {
            T *pResult = nullptr;
            if (S_OK == p->QueryInterface(
                iid, reinterpret_cast<void **>(&pResult)))
            {
                return MakeComPtr(pResult);
            }
            return nullptr;
        }
    };
}

#endif // SEVENZIPCORE_COMMONHELPER_H