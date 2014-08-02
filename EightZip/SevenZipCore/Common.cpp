#include "stdwx.h"
#include "Common.h"

#include <algorithm>

#include <boost/algorithm/string.hpp>

#include <Windows.h>

using namespace std;

namespace SevenZipCore
{
    TString Helper::GetFileName(const TString &tstrPath)
    {
        return tstrPath.substr(tstrPath.find_last_of(FOLDER_POSSIBLE_SEPARATOR) + 1);
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
        auto result = vector<TString>();
        result.reserve(vtstrPathPart.size());
        for (auto &value : vtstrPathPart)
        {
            if (!value.empty())
            {
                result.push_back(move(value));
            }
        }
        return result;
    }

    TString Helper::GetFilteredFileName(TString tstrPathPart)
    {
#ifdef __WINDOWS__
        static const TString tstrReserved(TEXT("<>:\"/\\|?*"));
        if (all_of(tstrPathPart.cbegin()
            , tstrPathPart.cend()
            , [](TCHAR value) { return value == TEXT('.'); }))
#else
        static const TString tstrReserved(TEXT("/"));
        if (tstrPathPart == TEXT(".") || tstrPathPart == TEXT(".."))
#endif
        {
            return TString();
        }
#ifdef __WINDOWS__
        boost::trim(tstrPathPart);
        while (!tstrPathPart.empty() && tstrPathPart.back() == TEXT('.'))
        {
            tstrPathPart.pop_back();
        }
#endif
        for (auto value : tstrPathPart)
        {
            if (tstrReserved.cend() != find(tstrReserved.cbegin(), tstrReserved.cend(), value))
            {
                value = TEXT('_');
            }

        }
        return tstrPathPart;
    }

    TString Helper::JoinString(std::vector<TString> vtstrPathPart, const TString &tstrSeparators, bool isSkipEmptyPart /*= false*/)
    {
        TString result;
        for (int i = 0; i != vtstrPathPart.size(); ++i)
        {
            const auto &value = vtstrPathPart[i];
            if (isSkipEmptyPart && value.empty())
            {
                continue;;
            }
            if (0 != i)
            {
                result.append(tstrSeparators);
            }
            result.append(value);
        }
        return result;
    }
}