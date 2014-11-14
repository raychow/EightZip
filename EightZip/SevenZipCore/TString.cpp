#include "TString.h"

#include <vector>

#include "Platform.h"

using namespace std;

wstring ConvertStringToWString(const string &value)
{
    LocaleSwitcher localeSwitcher("");
    size_t szLength = 0;
#ifdef __WINDOWS__
    if (0 != mbstowcs_s(&szLength, nullptr, 0, value.c_str(), _TRUNCATE))
    {
        return wstring();
    }
    auto vwchBuffer = vector<wchar_t>(szLength);
    if (0 != mbstowcs_s(
        &szLength, vwchBuffer.data(), szLength, value.c_str(), _TRUNCATE))
    {
        return wstring();
    }
    return wstring(vwchBuffer.data());
#else
    if ((size_t)-1 == (szLength = mbstowcs(nullptr, value.c_str(), 0)))
    {
        return wstring();
    }
    auto vwchBuffer = vector<wchar_t>(++szLength);
    mbstowcs(vwchBuffer.data(), value.c_str(), szLength);
    vwchBuffer[szLength - 1] = 0;
    return wstring(vwchBuffer.data());
#endif
}

string ConvertWStringToString(const wstring &value)
{
    LocaleSwitcher localeSwitcher("");
    size_t szLength = 0;
#ifdef __WINDOWS__
    if (0 != wcstombs_s(&szLength, nullptr, 0, value.c_str(), _TRUNCATE))
    {
        return string();
    }
    auto vchBuffer = vector<char>(szLength);
    if (0 != wcstombs_s(
        &szLength, vchBuffer.data(), szLength, value.c_str(), _TRUNCATE))
    {
        return string();
    }
    return string(vchBuffer.data());
#else
    if ((size_t)-1 == (szLength = wcstombs(nullptr, value.c_str(), 0)))
    {
        return string();
    }
    auto vchBuffer = vector<char>(++szLength);
    if ((size_t)-1 == wcstombs(vchBuffer.data(), value.c_str(), szLength))
    {
        return string();
    }
    vchBuffer[szLength - 1] = 0;
    return string(vchBuffer.data());
#endif
}