#include "stdwx.h"
#include "FileHelper.h"

#include <algorithm>
#include <memory>

#include "SevenZipCore/CommonHelper.h"

#include "Exception.h"

using namespace std;

namespace Helper
{
    void OpenFileExternal(const TString &tstrPath)
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

    int GetFileAttributes(TString tstrPath)
    {
        int status = 0;
#ifdef __WXMSW__
        //if (TRUE != ::PathFileExists(tstrPath.c_str()))
        //{
        //    return status;
        //}
        auto dwAttributes = ::GetFileAttributes(tstrPath.c_str());
        if (INVALID_FILE_ATTRIBUTES == dwAttributes)
        {
            return status;
        }
        if (dwAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            status |= EIGHT_FILE_STATUS_DIRECTORY;
        }
        else
        {
            status |= EIGHT_FILE_STATUS_FILE;
        }
        if (dwAttributes & FILE_ATTRIBUTE_REPARSE_POINT)
        {
            status |= EIGHT_FILE_STATUS_SYMBOLIC_LINK;
        }
#endif
        return status;
    }

    TString GetCanonicalPath(TString tstrPath)
    {
#ifdef __WXMSW__
        if (tstrPath.size() == 2 && tstrPath.back() == wxT(':'))
        {
            tstrPath.push_back(wxFILE_SEP_PATH);
        }
        int nBufferSize = ::GetFullPathName(tstrPath.c_str(), 0, nullptr, nullptr);
        if (!nBufferSize)
        {
            return tstrPath;
        }
        auto vwchFullPath = vector<wxChar>(nBufferSize);
        if (!::GetFullPathName(
            tstrPath.c_str(), nBufferSize, vwchFullPath.data(), nullptr))
        {
            return tstrPath;
        }
        nBufferSize = ::GetLongPathName(vwchFullPath.data(), nullptr, 0);
        if (!nBufferSize)
        {
            return vwchFullPath.data();
        }
        auto vwchLongPath = vector<wxChar>(nBufferSize);
        if (!::GetLongPathName(
            vwchFullPath.data(), vwchLongPath.data(), nBufferSize))
        {
            return vwchFullPath.data();
        }
        return vwchLongPath.data();
#endif
    }

    int GetCommonPrefixLength(
        const TString &tstr1,
        const TString &tstr2,
        bool isCaseSensitive /*= false*/)
    {
        auto szLength = min(tstr1.size(), tstr2.size());
        for (int i = 0; i != szLength; ++i)
        {
            if (ToTLower(tstr1[i]) != ToTLower(tstr2[i]))
            {
                return i;
            }
        }
        return szLength;
    }

    bool IsPathEqual(const TString &tstrLeft, const TString &tstrRight)
    {
        if (tstrLeft.size() != tstrRight.size())
        {
            return false;
        }
        auto szLength = tstrLeft.size();
        for (int i = 0; i != szLength; ++i)
        {
#ifdef __WXMSW__
            if (ToTLower(tstrLeft[i]) != ToTLower(tstrRight[i]))
#else
            if (tstrLeft[i] != tstrRight[i])
#endif
            {
                return false;
            }
        }
        return true;
    }

    TString GetLocation(const TString &tstrPath)
    {
        return tstrPath.substr(
            0, SevenZipCore::Helper::RemovePathSlash(tstrPath).find_last_of(
            wxFILE_SEP_PATH) + 1);
    }
}
