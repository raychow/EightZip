// FileInfo.h

#ifndef FILEINFO_H
#define FILEINFO_H

#include <map>

#include "SevenZipCore/TString.h"

class FileInfo
{
public:
    FileInfo() {}

    FileInfo(int nIconIndex, TString tstrTypeName)
        : m_nIconIndex(nIconIndex)
        , m_tstrTypeName(tstrTypeName)
    {}

    int GetIconIndex() const { return m_nIconIndex; }
    TString GetTypeName() const { return m_tstrTypeName; }

private:
    int m_nIconIndex = -1;
    TString m_tstrTypeName;
};

class FileInfoManager
{
public:
    static FileInfo GetFileInfo(bool isDir, const TString& tstrFileName = TString(), bool isVirtual = false);

private:
    static std::map<TString, FileInfo> m_mInfoCache;
};

#endif // FILEINFO_H
