// OpenCallback.h

#ifndef SEVENZIPCORE_OPENCALLBACK_H
#define SEVENZIPCORE_OPENCALLBACK_H

#include <boost/optional.hpp>

#include "COM.h"
#include "IArchive.h"
#include "IProgress.h"
#ifdef _DEBUG
#include "TString.h"
#endif

namespace SevenZipCore
{
    class OpenCallback
        : public IArchiveOpenCallback
        , public IProgress
    {
    public:
        virtual ~OpenCallback();

        void SetArchiveTotal(
            boost::optional<UINT64> numFiles, boost::optional<UINT64> numBytes);
        STDMETHOD(SetTotal)(const UINT64 *numFiles, const UINT64 *numBytes);
        
        void SetArchiveCompleted(
            boost::optional<UINT64> numFiles, boost::optional<UINT64> numBytes);
        STDMETHOD(SetCompleted)(const UINT64 *numFiles, const UINT64 *numBytes);

        void SetProgressTotal(UINT64 total);
        STDMETHOD(SetTotal)(UINT64 total);

        void SetProgressCompleted(boost::optional<UINT64> completeValue);
        STDMETHOD(SetCompleted)(const UINT64 *completeValue);

        IUNKNOWN_IMP1(IProgress)

    };
}

#endif // SEVENZIPCORE_OPENCALLBACK_H
