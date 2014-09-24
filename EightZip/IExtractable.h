// IExtractable.h

#ifndef IEXTRACTABLE_H
#define IEXTRACTABLE_H

#include <vector>

#include "SevenZipCore/BaseType.h"
#include "SevenZipCore/TString.h"

class IExtractable
{
public:
    virtual void Extract(TString tstrPath) const = 0;
    virtual TString Extract(
        UINT32 un32ArchiveIndex, TString tstrPath) const = 0;
    virtual void Extract(const std::vector<UINT32> &vun32ArchiveIndex,
        TString tstrPath) const = 0;

    virtual const TString &GetInternalPath() const = 0;

};

#endif // IEXTRACTABLE_H
