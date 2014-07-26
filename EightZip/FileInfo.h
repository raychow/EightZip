// FileInfo.h

#ifndef FILEINFO_H
#define FILEINFO_H

#include "SevenZipCore/TString.h"

extern wxDateTime GetTimeFromFileTime(_In_ CONST FILETIME *lpFileTime);

class FileInfo
{
public:
    FileInfo(TString tstrPath);

    bool IsOK() const { return m_isOK; }

    bool IsDirectory() const { return 0 != (m_fileAttributeData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY); }
    wxULongLong_t GetSize() const { return IsDirectory() ? 0 : wxULongLong(m_fileAttributeData.nFileSizeHigh, m_fileAttributeData.nFileSizeLow).GetValue(); }
    wxDateTime GetAccessed() const { return GetTimeFromFileTime(&m_fileAttributeData.ftLastAccessTime); }
    wxDateTime GetModified() const { return GetTimeFromFileTime(&m_fileAttributeData.ftLastWriteTime); }
    wxDateTime GetCreated() const { return GetTimeFromFileTime(&m_fileAttributeData.ftCreationTime); }

private:
    TString m_tstrPath;
    bool m_isOK;

    WIN32_FILE_ATTRIBUTE_DATA m_fileAttributeData;

};

#endif // FILEINFO_H
