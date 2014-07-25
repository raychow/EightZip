// EightDirFinder.h

#ifndef EIGHTDIRFINDER_H
#define EIGHTDIRFINDER_H

#include <memory>

#include "SevenZipCore/TString.h"

class FindCloser
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

class EightDirFinder
{
public:
    EightDirFinder(TString tstrDir);

    bool IsOpened() const;

    bool GetNext();

    TString GetFileName() const;
    bool IsDirectory() const;
    UINT64 GetFileSize() const;
    wxDateTime GetAccessed() const;
    wxDateTime GetModified() const;
    wxDateTime GetCreated() const;

private:
    std::unique_ptr<void, FindCloser> m_upHandle;
    WIN32_FIND_DATA m_findData;
    bool m_isFirst = true;

    static HANDLE __FindFirst();
};

#endif // EIGHTDIRFINDER_H
