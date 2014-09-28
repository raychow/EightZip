// TaskbarProgressHelper.h

#ifndef TASKBARPROGRESSHELPER_H
#define TASKBARPROGRESSHELPER_H

#include <ShObjIdl.h>

#include "SevenZipCore/BaseType.h"

#ifdef __WXMSW__

struct ITaskbarList3;

namespace Helper
{
    class TaskbarProgress
    {
    public:
        explicit TaskbarProgress(WXWidget handle);
        ~TaskbarProgress();

        void SetValue(UINT64 un64Completed, UINT64 un64Total) const;
        void SetState(TBPFLAG tbpFlags) const;

    private:
        WXWidget m_handle;
        ITaskbarList3 *m_pTaskbarList = nullptr;

    };
}

#endif

#endif // TASKBARPROGRESSHELPER_H
