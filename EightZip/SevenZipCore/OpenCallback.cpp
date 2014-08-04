#include "OpenCallback.h"

#include <cassert>

#include "Exception.h"

using namespace std;
using namespace boost;

namespace SevenZipCore
{
    OpenCallback::~OpenCallback()
    {

    }

    void OpenCallback::SetArchiveTotal(
        boost::optional<UINT64> numFiles, boost::optional<UINT64> numBytes)
    {
        // COpenArchiveCallback::SetTotal
    }

    STDMETHODIMP OpenCallback::SetTotal(
        const UINT64 *numFiles, const UINT64 *numBytes)
    {
        try
        {
            SetArchiveTotal(numFiles ? optional<UINT64>(*numFiles) : none,
                numBytes ? optional<UINT64>(*numBytes) : none);
            return S_OK;
        }
        catch (...)
        {
            return E_FAIL;
        }
    }

    void OpenCallback::SetArchiveCompleted(
        boost::optional<UINT64> numFiles, boost::optional<UINT64> numBytes)
    {
        // COpenArchiveCallback::SetCompleted
    }

    STDMETHODIMP OpenCallback::SetCompleted(
        const UINT64 *numFiles, const UINT64 *numBytes)
    {
        try
        {
            SetArchiveCompleted(numFiles ? optional<UINT64>(*numFiles) : none,
                numBytes ? optional<UINT64>(*numBytes) : none);
            return S_OK;
        }
        catch (...)
        {
            return E_FAIL;
        }
    }

    void OpenCallback::SetProgressTotal(UINT64 total)
    {
        // FM, COpenArchiveCallback::SetTotal
    }

    STDMETHODIMP OpenCallback::SetTotal(UINT64 total)
    {
        try
        {
            SetProgressTotal(total);
            return S_OK;
        }
        catch (...)
        {
            return E_FAIL;
        }
    }

    void OpenCallback::SetProgressCompleted(
        boost::optional<UINT64> completeValue)
    {
        // FM, COpenArchiveCallback::SetCompleted
    }

    STDMETHODIMP OpenCallback::SetCompleted(const UINT64 *completeValue)
    {
        try
        {
            SetProgressCompleted(
                completeValue ? optional<UINT64>(*completeValue) : none);
            return S_OK;
        }
        catch (...)
        {
            return E_FAIL;
        }
    }
}