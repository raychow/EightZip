#include "TString.h"

#include <memory>

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
    unique_ptr<wchar_t[]> upWChar(new wchar_t[szLength]);
    if (0 != mbstowcs_s(
        &szLength, upWChar.get(), szLength, value.c_str(), _TRUNCATE))
    {
        return wstring();
    }
    return wstring(upWChar.get());
#else
    if ((size_t)-1 == (szLength = mbstowcs(nullptr, value.c_str(), 0)))
    {
        return wstring();
    }
    unique_ptr<wchar_t[]> upWChar(new wchar_t[++szLength]);
    mbstowcs(upWChar.get(), value.c_str(), szLength);
    upWChar[szLength - 1] = 0;
    return wstring(upWChar.get());
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
    unique_ptr<char[]> upChar(new char[szLength]);
    if (0 != wcstombs_s(
        &szLength, upChar.get(), szLength, value.c_str(), _TRUNCATE))
    {
        return string();
    }
    return string(upChar.get());
#else
    if ((size_t)-1 == (szLength = wcstombs(nullptr, value.c_str(), 0)))
    {
        return string();
    }
    unique_ptr<char[]> upChar(new char[++szLength]);
    if ((size_t)-1 == wcstombs(upChar.get(), value.c_str(), szLength))
    {
        return string();
}
    upChar[szLength - 1] = 0;
    return string(upChar.get());
#endif
}