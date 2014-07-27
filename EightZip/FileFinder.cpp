#include "stdwx.h"
#include "FileFinder.h"

using namespace std;

FileFinder::FileFinder(TString tstrDir, bool isDiretoryOnly /*= false*/)
    : m_isDirectoryOnly(isDiretoryOnly)
{
    if (wxEndsWithPathSeparator(tstrDir))
    {
        tstrDir.append(wxT("*"));
    }
    else
    {
        tstrDir.append(wxT("\\*"));
    }
#ifdef __WXMSW__
    m_upHandle.reset(::FindFirstFile(tstrDir.c_str(), &m_findData));
#endif
}

void FileFinder::Close()
{
#ifdef __WXMSW__
	m_upHandle.reset();
#endif
}

bool FileFinder::IsOpened() const
{
#ifdef __WXMSW__
	return m_upHandle && m_upHandle.get() != INVALID_HANDLE_VALUE;
#endif
}

bool FileFinder::FindNext()
{
    if (!IsOpened())
    {
        return false;
    }

    while (true)
    {
        if (m_isFirst)
        {
            m_isFirst = false;
        }
        else
        {
            if (!__FindNext())
            {
                Close();
                return false;
            }
        }
        if (__Check())
        {
            return true;
        }
    }
}

bool FileFinder::__FindNext()
{
#ifdef __WXMSW__
	return FALSE != ::FindNextFile(m_upHandle.get(), &m_findData);
#endif
}

bool FileFinder::__Check()
{
    if (GetFileName() == wxT(".") || GetFileName() == wxT(".."))
    {
        return false;
    }
    return true;
}

