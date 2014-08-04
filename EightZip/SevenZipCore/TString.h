// TString.h

#ifndef SEVENZIPCORE_TSTRING_H
#define SEVENZIPCORE_TSTRING_H

#include <locale>
#include <string>
#include <sstream>

#ifdef _UNICODE

typedef std::wstring TString;
typedef std::wstringstream TStringStream;

#define ToTUpper towupper
#define ToTLower towlower

#define ToTString std::to_wstring

#else

typedef std::string TString;
typedef std::stringstream TStringStream;

#define ToTUpper toupper
#define ToTLower tolower

#define ToTString std::to_string

#endif

std::wstring ConvertStringToWString(const std::string &value);
std::string ConvertWStringToString(const std::wstring &value);

std::wstring ConvertTStringToWString(const TString &value);
std::string ConvertTStringToString(const TString &value);

std::wstring ConvertStringToTString(const std::string &value);
std::wstring ConvertWStringToTString(const std::wstring &value);

class LocaleSwitcher
{
public:
    LocaleSwitcher(const char *locale)
        : m_strOldLocale(setlocale(LC_CTYPE, nullptr))
    {
        setlocale(LC_CTYPE, locale);
    }
    ~LocaleSwitcher()
    {
        setlocale(LC_CTYPE, m_strOldLocale.c_str());
    }

private:
    std::string m_strOldLocale;
};

#endif // SEVENZIPCORE_TSTRING_H