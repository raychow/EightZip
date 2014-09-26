// IExtractIndicator.h

#ifndef IEXTRACTINDICATOR_H
#define IEXTRACTINDICATOR_H

#include <boost/optional.hpp>

#include "BaseType.h"
#include "IArchive.h"
#include "TString.h"

namespace SevenZipCore
{
    class IExtractIndicator
    {
    public:
        virtual void SetTotal(UINT64 un64Total) = 0;
        virtual void SetCompleted(boost::optional<UINT64> oun64Value) = 0;
        virtual OverwriteAnswer AskOverwrite(TString tstrExistPath,
            boost::optional<FILETIME> oftExistModified,
            boost::optional<UINT64> un64ExistSize,
            TString tstrNewPath,
            boost::optional<FILETIME> oftNewModified,
            boost::optional<UINT64> un64NewSize) = 0;
        virtual void AddError(TString tstrMessage) = 0;
        virtual void AddError(TString tstrMessage, TString tstrParameter) = 0;
        virtual void Prepare(TString tstrPath,
            bool isFolder,
            ExtractAskMode askMode,
            boost::optional<UINT64> oun64Position) = 0;
        virtual void SetOperationResult(ExtractResult extractResult) = 0;

        virtual ~IExtractIndicator() = 0 { };

    };
}

#endif // IEXTRACTINDICATOR_H