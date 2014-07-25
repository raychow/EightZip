#include "stdwx.h"
#include "TString.h"

#include <memory>

using namespace std;

wstring ConvertStringToWString(const std::string &value)
{
    LocaleSwitcher localeSwitcher("");
    auto szLength = mbstowcs(nullptr, value.c_str(), 0);
    if ((size_t)-1 == szLength)
    {
        return wstring();
    }
    unique_ptr<wchar_t[]> upWChar(new wchar_t[++szLength]);
    mbstowcs(upWChar.get(), value.c_str(), szLength);
    return std::wstring(upWChar.get());
}

string ConvertWStringToString(const std::wstring &value)
{
    LocaleSwitcher localeSwitcher("");
    auto szLength = (value.size() + 1) * sizeof(wchar_t);
    unique_ptr<char[]> upChar(new char[szLength]);
    if ((size_t)-1 == wcstombs(upChar.get(), value.c_str(), szLength))
    {
        return string();
    }
    return std::string(upChar.get());
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

LocaleSwitcher::LocaleSwitcher(const char *locale)
    : m_strOldLocale(setlocale(LC_CTYPE, nullptr))
{
    setlocale(LC_CTYPE, locale);
}

LocaleSwitcher::~LocaleSwitcher()
{
    setlocale(LC_CTYPE, m_strOldLocale.c_str());
}
