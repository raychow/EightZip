#include "stdwx.h"
#include "FileInfo.h"

using namespace std;

FileInfo::FileInfo(TString tstrPath)
    : m_tstrPath(move(tstrPath))
{
    m_isOK = (FALSE != ::GetFileAttributesEx(m_tstrPath.c_str(), GetFileExInfoStandard, &m_fileAttributeData));
}

wxDateTime GetTimeFromFileTime(_In_ CONST FILETIME *lpFileTime)
{
    SYSTEMTIME systemTime;
    FileTimeToSystemTime(lpFileTime, &systemTime);
    return wxDateTime(systemTime);
}