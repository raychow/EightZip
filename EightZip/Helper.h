// Helper.h

#ifndef HELPER_H
#define HELPER_H

#include "SevenZipCore/TString.h"

#define DEFINE_ATTRIBUTES(name, value) \
    inline FileAttributes &name(bool isSet = true) \
    { \
        __Set(value, isSet); \
        return *this; \
    } \
    inline bool Is##name() const \
    { \
        return 0 != (m_status & value); \
    } \

class FileAttributes
{
    enum Status
    {
        EIGHT_FILE_STATUS_DIRECTORY = 0x01,
        EIGHT_FILE_STATUS_FILE = 0x02,
        EIGHT_FILE_STATUS_SYMBOLIC_LINK = 0x04,
    };

public:
    DEFINE_ATTRIBUTES(Directory, EIGHT_FILE_STATUS_DIRECTORY)
    DEFINE_ATTRIBUTES(File, EIGHT_FILE_STATUS_FILE)
    DEFINE_ATTRIBUTES(SymbolicLink, EIGHT_FILE_STATUS_SYMBOLIC_LINK)

private:
    int m_status = 0;

    void __Set(Status value, bool isSet)
    {
        if (isSet)
        {
            m_status |= value;
        }
        else
        {
            m_status &= ~value;
        }
    }

};

namespace Helper
{
    void OpenFileExternal(const TString &tstrPath);
    wxSizer *AlignBorder(wxSizer *pSizer);
    FileAttributes GetFileAttributes(TString tstrPath);
    TString GetCanonicalPath(TString tstrPath);
    int GetCommonPrefixLength(const TString &tstr1,
        const TString &tstr2,
        bool isCaseSensitive = false);
    bool IsPathEqual(const TString &tstrLeft, const TString &tstrRight);
    TString GetParentPath(const TString &tstrPath);

};

#endif // HELPER_H
