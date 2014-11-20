// IOpenIndicator.h

#ifndef IOPENINDICATOR_H
#define IOPENINDICATOR_H

#include <boost/optional.hpp>

#include "Platform.h"

#include "TString.h"

namespace SevenZipCore
{
    class IOpenIndicator
    {
    public:
        virtual void SetTotal(UINT64 un64FileCount, UINT64 un64Size) = 0;
        virtual void SetCompleted(UINT64 un64FileCount, UINT64 un64Size) = 0;
        virtual boost::optional<TString> GetPassword() = 0;

        virtual ~IOpenIndicator() = 0 {}

    };
}

#endif // IOPENINDICATOR_H
