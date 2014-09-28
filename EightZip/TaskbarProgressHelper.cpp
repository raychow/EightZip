#include "stdwx.h"

#ifdef __WXMSW__

#include "TaskbarProgressHelper.h"

#include <VersionHelpers.h>

namespace Helper
{
    TaskbarProgress::TaskbarProgress(WXWidget handle)
        : m_handle(handle)
    {
        if (handle && IsWindows7OrGreater())
        {
            if (!SUCCEEDED(CoCreateInstance(CLSID_TaskbarList,
                nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_pTaskbarList))))
            {
                m_pTaskbarList = nullptr;
            }
        }
    }

    TaskbarProgress::~TaskbarProgress()
    {
        if (m_pTaskbarList)
        {
            m_pTaskbarList->SetProgressState(m_handle, TBPF_NOPROGRESS);
        }
    }

    void TaskbarProgress::SetValue(
        UINT64 un64Completed, UINT64 un64Total) const
    {
        if (!m_pTaskbarList)
        {
            return;
        }
        m_pTaskbarList->SetProgressValue(m_handle, un64Completed, un64Total);
    }

    void TaskbarProgress::SetState(TBPFLAG tbpFlags) const
    {
        if (!m_pTaskbarList)
        {
            return;
        }
        m_pTaskbarList->SetProgressState(m_handle, tbpFlags);
    }
}

#endif
