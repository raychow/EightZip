// FileFinder.h

#ifndef FILEFINDER_H
#define FILEFINDER_H

#include <memory>

#include "SevenZipCore/TString.h"

extern wxDateTime GetTimeFromFileTime(_In_ CONST FILETIME *lpFileTime);

class FileFinder
{
public:
    FileFinder(TString tstrDir, bool isDiretoryOnly = false);

    void Close();
    bool IsOpened() const;

    bool FindNext();

    TString GetFileName() const { return m_findData.cFileName; }
    bool IsDirectory() const { return (m_findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0; }
    wxULongLong_t GetSize() const { return IsDirectory() ? 0 : wxULongLong(m_findData.nFileSizeHigh, m_findData.nFileSizeLow).GetValue(); }
    wxDateTime GetAccessed() const { return GetTimeFromFileTime(&m_findData.ftLastAccessTime); }
    wxDateTime GetModified() const { return GetTimeFromFileTime(&m_findData.ftLastWriteTime); }
    wxDateTime GetCreated() const { return GetTimeFromFileTime(&m_findData.ftCreationTime); }

private:
    class FinderCloser
    {
    public:
        void operator()(HANDLE handle)
        {
            if (handle != INVALID_HANDLE_VALUE)
            {
                FindClose(handle);
            }
        }
    };

    bool m_isDirectoryOnly;
    std::unique_ptr<void, FinderCloser> m_upHandle;
    WIN32_FIND_DATA m_findData;
    bool m_isFirst = true;

    bool __FindNext();
    bool __Check();


};

#endif // FILEFINDER_H
