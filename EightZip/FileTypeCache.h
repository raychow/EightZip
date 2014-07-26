// FileTypeCache.h

#ifndef FILETYPECACHE_H
#define FILETYPECACHE_H

#include <map>

#include "SevenZipCore/TString.h"

class FileTypeInfo
{
public:
    FileTypeInfo() {}
    FileTypeInfo(int nIconIndex, TString tstrName)
        : m_nIconIndex(nIconIndex)
        , m_tstrName(tstrName)
    {}

    int GetIconIndex() const { return m_nIconIndex; }
    TString GetName() const { return m_tstrName; }

private:
    int m_nIconIndex;
    TString m_tstrName;
};

class FileTypeCache
{
public:
    static FileTypeInfo GetInfo(bool isDirectory, TString tstrFileName = TString());

private:
    static std::map<TString, FileTypeInfo> m_mInfoCache;

};

#endif // FILETYPECACHE_H
