#include "TString.h"

#include <memory>

#include "Platform.h"

using namespace std;

wstring ConvertStringToWString(const std::string &value)
{
    LocaleSwitcher localeSwitcher("");
    size_t szLength = 0;
#ifdef __WINDOWS__
    if (0 != mbstowcs_s(&szLength, nullptr, 0, value.c_str(), _TRUNCATE))
    {
        return wstring();
    }
    unique_ptr<wchar_t[]> upWChar(new wchar_t[szLength]);
    if (0 != mbstowcs_s(&szLength, upWChar.get(), szLength, value.c_str(), _TRUNCATE))
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
    return std::wstring(upWChar.get());
#endif
}

string ConvertWStringToString(const std::wstring &value)
{
    LocaleSwitcher localeSwitcher("");
    size_t szLength = 0;
#ifdef __WINDOWS__
    if (0 != wcstombs_s(&szLength, nullptr, 0, value.c_str(), _TRUNCATE))
    {
        return std::string();
    }
    unique_ptr<char[]> upChar(new char[szLength]);
    if (0 != wcstombs_s(&szLength, upChar.get(), szLength, value.c_str(), _TRUNCATE))
    {
        return std::string();
    }
    return std::string(upChar.get());
#else
    if ((size_t)-1 == (szLength = wcstombs(nullptr, value.c_str(), 0)))
    {
        return std::string();
    }
    unique_ptr<char[]> upChar(new char[++szLength]);
    if ((size_t)-1 == wcstombs(upChar.get(), value.c_str(), szLength))
    {
        return std::string();
}
    upChar[szLength - 1] = 0;
    return std::string(upChar.get());
#endif
}

wstring ConvertTStringToWString(const TString &value)
{
#ifdef _UNICODE
    return value;
#else
    return ConvertStringToWString(value);
#endif
}

string ConvertTStringToString(const TString &value)
{
#ifdef _UNICODE
    return ConvertWStringToString(value);
#else
    return value;
#endif
}

wstring ConvertStringToTString(const std::string &value)
{
#ifdef _UNICODE
    return ConvertStringToWString(value);
#else
    return value;
#endif
}

wstring ConvertWStringToTString(const std::wstring &value)
{
#ifdef _UNICODE
    return value;
#else
    return ConvertWStringToString(value);
#endif
}
