// String.h

#ifndef STRING_H
#define STRING_H

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
    LocaleSwitcher(const char *locale);
    ~LocaleSwitcher();

private:
    std::string m_strOldLocale;
};

#endif // STRING_H
