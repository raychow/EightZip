// CommonHelper.h

#ifndef SEVENZIPCORE_COMMONHELPER_H
#define SEVENZIPCORE_COMMONHELPER_H

#include <memory>
#include <vector>

#include <boost/optional.hpp>

#include "ComPtr.h"
#include "Platform.h"
#include "TString.h"

namespace SevenZipCore
{
    class PropertyVariant;

    namespace Helper
    {
        TString GetFileName(const TString &tstrPath);
        TString GetFileNameStem(const TString &tstrPath);
        TString GetFileExtension(const TString &tstrExtension);

        TString ToUpper(const TString &value);
        TString ToLower(const TString &value);

        TString MakePathSlash(TString value);
        TString RemovePathSlash(TString value);
        bool IsEndOfSlash(const TString &value);
        std::vector<TString> SplitString(
            const TString &tstrPath,
            const TString &tstrSeparators,
            bool isSkipEmptyPart = false);
        std::vector<TString> GetFilteredPath(
            std::vector<TString> vtstrPathPart);
        TString GetFilteredFileName(TString tstrPathPart);
        TString JoinString(
            std::vector<TString> vtstrPathPart,
            const TString &tstrSeparators,
            bool isSkipEmptyPart = false);
        void SetFileTime(
            TString tstrPath,
            const FILETIME *lpCreationTime,
            const FILETIME *lpLastAccessTime,
            const FILETIME *lpLastWriteTime);
        boost::optional<FILETIME> GetFileModifiedTime(TString tstrPath);
        bool AutoRenamePath(TString &tstrPath);
        time_t GetUnixTimeFromFileTime(const FILETIME &fileTime);

        template<typename T, typename C>
        SevenZipCore::unique_com_ptr<T> QueryInterface(C &c, REFGUID iid)
        {
            T *pResult = nullptr;
            if (S_OK == c.QueryInterface(
                iid, reinterpret_cast<void **>(&pResult)))
            {
                return MakeUniqueCom(pResult, false);
            }
            return nullptr;
        }
    };
}

#endif // SEVENZIPCORE_COMMONHELPER_H
