// IStreamAdapter.h

#ifndef SEVENZIPCORE_ISTREAMADAPTER_H
#define SEVENZIPCORE_ISTREAMADAPTER_H

#include <memory>
#include <vector>

#include "Adapter.h"
#include "IStream.h"

#define DECLARE_IINSTREAM_ADAPTER \
    std::vector<char> Read(UINT32 size); \
    UINT64 Seek(INT64 offset, UINT32 seekOrigin); \

#define IMPLEMENT_IINSTREAM_ADAPTER(target_name) \
    std::vector<char> target_name##Adapter::Read(UINT32 size) \
    { \
        std::vector<char> result(size); \
        UINT32 unReadSize = 0; \
        CHECK_OK(m_spTarget->Read(result.data(), size, &unReadSize), \
            StreamException, \
            "Cannot read the file stream."); \
        result.resize(size); \
        return result; \
    } \
    UINT64 target_name##Adapter::Seek(INT64 offset, UINT32 seekOrigin) \
    { \
        UINT64 result = 0; \
        CHECK_OK(m_spTarget->Seek(offset, seekOrigin, &result), \
            StreamException, \
            "Cannot seek the file stream."); \
        return result; \
    } \

#define DECLARE_ISTREAMGETSIZE_ADAPTER \
    UINT64 GetSize(); \

#define IMPLEMENT_ISTREAMGETSIZE_ADAPTER(target_name) \
    UINT64 target_name##Adapter::GetSize() \
    { \
        UINT64 result = 0; \
        CHECK_OK(m_spTarget->GetSize(&result), \
            StreamException, \
            "Cannot get size of the file stream."); \
        return result; \
    } \

namespace SevenZipCore
{
    DECLARE_ADAPTER_CLASS1(IInStream, DECLARE_IINSTREAM_ADAPTER)
    DECLARE_ADAPTER_CLASS1(IStreamGetSize, DECLARE_ISTREAMGETSIZE_ADAPTER)
}

#endif // SEVENZIPCORE_ISTREAMADAPTER_H
