#include "stdwx.h"
#include "Common.h"

#include <algorithm>

#include <Windows.h>

using namespace std;

namespace SevenZipCore
{
    TString Helper::GetFileName(const TString &tstrPath)
    {
        return tstrPath.substr(tstrPath.find_last_of(FOLDER_SEPARATOR) + 1);
    }

    TString Helper::GetFileNameStem(const TString &tstrPath)
    {
        TString tstrFileName = GetFileName(tstrPath);
        return tstrFileName.substr(0, tstrFileName.rfind(TEXT(".")));
    }

    TString Helper::GetFileExtension(const TString &tstrExtension)
    {
        return tstrExtension.substr(tstrExtension.rfind(TEXT(".")) + 1);
    }

    TString Helper::ToUpper(const TString &value)
    {
        TString result;
        result.resize(value.size());
        transform(value.cbegin(), value.cend(), result.begin(), ToTUpper);
        return result;
    }

    TString Helper::ToLower(const TString &value)
    {
        TString result;
        result.resize(value.size());
        transform(value.cbegin(), value.cend(), result.begin(), ToTLower);
        return result;
    }

    std::vector<TString> Helper::SplitString(const TString &tstrPath, const TString &tstrSeparators, bool isSkipEmptyPart /*= false*/)
    {
        vector<TString> vtstrResult;
        TString::size_type stPosition = -1;
        TString::size_type stLastPosition = 0;
        while ((stPosition = tstrPath.find_first_of(tstrSeparators, stPosition + 1)) != string::npos)
        {
            if (0 != stPosition - stLastPosition)
            {
                vtstrResult.push_back(tstrPath.substr(stLastPosition, stPosition - stLastPosition));
            }
            stLastPosition = stPosition + 1;
        }
        vtstrResult.push_back(tstrPath.substr(stLastPosition));
        return vtstrResult;
    }

    vector<TString> Helper::GetFilteredPath(vector<TString> vtstrPathPart)
    {
        for (auto &tstrPathPart : vtstrPathPart)
        {
            tstrPathPart = GetFilteredFileName(move(tstrPathPart));
        }
    }

    TString Helper::GetFilteredFileName(TString tstrPathPart)
    {
        static const TCHAR *pchSepical = TEXT("*?<>|:\"");
        if (all_of(tstrPathPart.cbegin()
            , tstrPathPart.cend()
            , [](TCHAR value) { return value == TEXT('.'); }))
        {
            return TString();
        }
        for (auto value : tstrPathPart)
        {

        }
    }

}