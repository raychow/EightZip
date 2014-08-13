// FileInfo.h

#ifndef FILEINFO_H
#define FILEINFO_H

#include <map>

#include "SevenZipCore/TString.h"

#ifdef __WXMSW__
extern wxDateTime GetTimeFromFileTime(_In_ CONST FILETIME *lpFileTime);
#endif

class FileInfo
{
public:
    FileInfo(TString tstrPath);

    bool IsOK() const { return m_isOK; }

    TString GetCanonicalPath() const;
    bool IsDirectory() const
    {
        return 0 != (m_fileAttributeData.dwFileAttributes
            & FILE_ATTRIBUTE_DIRECTORY);
    }
    wxULongLong_t GetSize() const
    {
        return IsDirectory() ? 0 : wxULongLong(
            m_fileAttributeData.nFileSizeHigh,
            m_fileAttributeData.nFileSizeLow).GetValue();
    }
    wxDateTime GetAccessed() const
    {
        return GetTimeFromFileTime(&m_fileAttributeData.ftLastAccessTime);
    }
    wxDateTime GetModified() const
    {
        return GetTimeFromFileTime(&m_fileAttributeData.ftLastWriteTime);
    }
    wxDateTime GetCreated() const
    {
        return GetTimeFromFileTime(&m_fileAttributeData.ftCreationTime);
    }
    TString GetType() const
    {
        return GetType(m_tstrPath, IsDirectory(), false);
    }
    static TString GetType(
        TString tstrFileName,
        bool isDirectory = false,
        bool isVirtual = true);
    static int GetIconIndex(
        TString tstrFileName,
        bool isDirectory = false,
        bool isVirtual = true);

private:
    TString m_tstrPath;
    mutable TString m_tstrCanonicalPath;
    bool m_isOK;

    WIN32_FILE_ATTRIBUTE_DATA m_fileAttributeData;

    static std::map<TString, TString> m_mTypeCache;

};

#endif // FILEINFO_H
