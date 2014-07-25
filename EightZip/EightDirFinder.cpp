#include "stdwx.h"
#include "EightDirFinder.h"

using namespace std;

EightDirFinder::EightDirFinder(TString tstrDir)
{
    if (wxEndsWithPathSeparator(tstrDir))
    {
        tstrDir.append(wxT("*"));
    }
    else
    {
        tstrDir.append(wxT("\\*"));
    }
    m_upHandle.reset(::FindFirstFile(tstrDir.c_str(), &m_findData));
}

bool EightDirFinder::IsOpened() const
{
    return m_upHandle && m_upHandle.get() != INVALID_HANDLE_VALUE;
}

bool EightDirFinder::GetNext()
{
    if (!IsOpened())
    {
        return false;
    }
    if (m_isFirst)
    {
        m_isFirst = false;
        return true;
    }

    if (!::FindNextFile(m_upHandle.get(), &m_findData))
    {
        m_upHandle.reset();
    }
    return true;
}

TString EightDirFinder::GetFileName() const
{
    return m_findData.cFileName;
}

bool EightDirFinder::IsDirectory() const
{
    return (m_findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

UINT64 EightDirFinder::GetFileSize() const
{
    return IsDirectory() ? 0 : wxULongLong(m_findData.nFileSizeHigh, m_findData.nFileSizeLow).GetValue();
}

wxDateTime EightDirFinder::GetAccessed() const
{
    SYSTEMTIME systemTime;
    FileTimeToSystemTime(&m_findData.ftLastAccessTime, &systemTime);
    return wxDateTime(systemTime);
}

wxDateTime EightDirFinder::GetModified() const
{
    SYSTEMTIME systemTime;
    FileTimeToSystemTime(&m_findData.ftLastWriteTime, &systemTime);
    return wxDateTime(systemTime);
}

wxDateTime EightDirFinder::GetCreated() const
{
    SYSTEMTIME systemTime;
    FileTimeToSystemTime(&m_findData.ftCreationTime, &systemTime);
    return wxDateTime(systemTime);
}

