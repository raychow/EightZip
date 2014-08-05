// ICoderAdapter.h

#ifndef SEVENZIPCORE_ICODERADAPTER_H
#define SEVENZIPCORE_ICODERADAPTER_H

#include <memory>

#include "Adapter.h"
#include "ICoder.h"

#define DELCARE_ISETCOMPRESSCODECSINFO_ADAPTER \
    void SetCompressCodecsInfo(ICompressCodecsInfo &compressCodecsInfo) const; \

#define IMPLEMENT_ISETCOMPRESSCODECSINFO_ADAPTER(target_name) \
    void target_name##Adapter::SetCompressCodecsInfo( \
        ICompressCodecsInfo &compressCodecsInfo) const \
    { \
        if (S_OK != m_spTarget->SetCompressCodecsInfo(&compressCodecsInfo)) \
            throw LibraryException("Cannot set compress codecs info."); \
    } \

namespace SevenZipCore
{
    DECLARE_ADAPTER_CLASS1(
        ISetCompressCodecsInfo, DELCARE_ISETCOMPRESSCODECSINFO_ADAPTER)
}

#endif // SEVENZIPCORE_ICODERADAPTER_H
