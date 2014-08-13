#include "stdwx.h"
#include "Helper.h"

#include <memory>

#include "Exception.h"

using namespace std;

void Helper::OpenFileExternal(const TString &tstrPath)
{
#ifdef __WXMSW__
    if ((int)::ShellExecute(
        nullptr, nullptr, tstrPath.c_str(), nullptr, nullptr, SW_SHOWNORMAL
        ) > 32)
    {
        return;
    }
#else
#endif
    throw SystemException("Cannot execute the specified file.");
}

wxSizer *Helper::AlignBorder(wxSizer *pSizer)
{
#ifdef __WXMSW__
    int nCount = pSizer->GetItemCount();
    if (nCount)
    {
        auto pSizerLast = pSizer->GetItem(nCount - 1);
        auto nBorderLast = pSizerLast->GetBorder();
        if ((pSizerLast->GetFlag() & wxRIGHT) && SIZER_BORDER > nBorderLast)
        {
            pSizer->AddSpacer(SIZER_BORDER - pSizerLast->GetBorder());
        }
    }
#endif
    return pSizer;
}

FileAttributes Helper::GetFileAttributes(TString tstrPath)
{
    FileAttributes status;
#ifdef __WXMSW__
    //if (TRUE != ::PathFileExists(tstrPath.c_str()))
    //{
    //    return status;
    //}
    auto dwAttributes = ::GetFileAttributes(tstrPath.c_str());
    if (INVALID_FILE_ATTRIBUTES == dwAttributes)
    {
        return status;
    }
    if (dwAttributes & FILE_ATTRIBUTE_DIRECTORY)
    {
        status.Directory();
    }
    else
    {
        status.File();
    }
    if (dwAttributes & FILE_ATTRIBUTE_REPARSE_POINT)
    {
        status.SymbolicLink();
    }
#endif
    return status;
}

TString Helper::GetCanonicalPath(TString tstrPath)
{
#ifdef __WXMSW__
    int nBufferSize = ::GetFullPathName(tstrPath.c_str(), 0, nullptr, nullptr);
    if (!nBufferSize)
    {
        return tstrPath;
    }
    unique_ptr<wxChar[]> uptchFullPath(new wxChar[nBufferSize]);
    if (!::GetFullPathName(
        tstrPath.c_str(), nBufferSize, uptchFullPath.get(), nullptr))
    {
        return tstrPath;
    }
    nBufferSize = ::GetLongPathName(uptchFullPath.get(), nullptr, 0);
    if (!nBufferSize)
    {
        return uptchFullPath.get();
    }
    unique_ptr<wxChar[]> uptchLongPath(new wxChar[nBufferSize]);
    if (!::GetLongPathName(
        uptchFullPath.get(), uptchLongPath.get(), nBufferSize))
    {
        return uptchFullPath.get();
    }
    return uptchLongPath.get();
#endif
}
