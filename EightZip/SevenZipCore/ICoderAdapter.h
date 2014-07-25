// ICoderAdapter.h

#ifndef ICODERADAPTER_H
#define ICODERADAPTER_H

#include <memory>

#include "Adapter.h"
#include "ICoder.h"

#define DELCARE_ISETCOMPRESSCODECSINFO_ADAPTER \
    void SetCompressCodecsInfo(ICompressCodecsInfo &compressCodecsInfo); \

#define IMPLEMENT_ISETCOMPRESSCODECSINFO_ADAPTER(target_name) \
    void target_name##Adapter::SetCompressCodecsInfo(ICompressCodecsInfo &compressCodecsInfo) \
    { \
        if (S_OK != m_spTarget->SetCompressCodecsInfo(&compressCodecsInfo)) \
            throw CodecException("Cannot set compress codecs info."); \
    } \

namespace SevenZipCore
{
    DECLARE_ADAPTER_CLASS1(ISetCompressCodecsInfo, DELCARE_ISETCOMPRESSCODECSINFO_ADAPTER)
}

#endif // ICODERADAPTER_H
