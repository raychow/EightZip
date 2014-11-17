// OpenCallback.h

#ifndef SEVENZIPCORE_OPENCALLBACK_H
#define SEVENZIPCORE_OPENCALLBACK_H

#include <boost/optional.hpp>

#include "COM.h"
#include "IArchive.h"
#include "IPassword.h"
#include "IProgress.h"
#ifdef _DEBUG
#include "TString.h"
#endif

namespace SevenZipCore
{
    class IOpenIndicator;

    class OpenCallback
        : public IArchiveOpenCallback
        , public ICryptoGetTextPassword
        , public IProgress
    {
    public:
        OpenCallback(IOpenIndicator *pOpenIndicator)
            : m_pOpenIndicator(pOpenIndicator)
        {
        }
        virtual ~OpenCallback() {}

        STDMETHOD(SetTotal)(const UINT64 *numFiles, const UINT64 *numBytes);
        STDMETHOD(SetCompleted)(const UINT64 *numFiles, const UINT64 *numBytes);

        STDMETHOD(CryptoGetTextPassword)(BSTR *password);

        STDMETHOD(SetTotal)(UINT64 total);
        STDMETHOD(SetCompleted)(const UINT64 *completeValue);

        IUNKNOWN_IMP3(IArchiveOpenCallback,
            ICryptoGetTextPassword,
            IProgress)

    private:
        IOpenIndicator *m_pOpenIndicator = nullptr;

    };
}

#endif // SEVENZIPCORE_OPENCALLBACK_H
