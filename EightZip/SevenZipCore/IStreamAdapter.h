// IStreamAdapter.h

#ifndef SEVENZIPCORE_ISTREAMADAPTER_H
#define SEVENZIPCORE_ISTREAMADAPTER_H

#include <memory>
#include <vector>

#include "Adapter.h"
#include "IStream.h"

#define DECLARE_IINSTREAM_ADAPTER \
    std::vector<char> Read(UINT32 size) const; \
    UINT64 Seek(INT64 offset, UINT32 seekOrigin) const; \

#define IMPLEMENT_IINSTREAM_ADAPTER(target_name) \
    std::vector<char> target_name##Adapter::Read(UINT32 size) const \
    { \
        std::vector<char> result(size); \
        UINT32 unReadSize = 0; \
        CHECK_OK(m_spTarget->Read(result.data(), size, &unReadSize), \
            StreamException, \
            "Cannot read the file stream."); \
        result.resize(size); \
        return result; \
    } \
    UINT64 target_name##Adapter::Seek(INT64 offset, UINT32 seekOrigin) const \
    { \
        UINT64 result = 0; \
        CHECK_OK(m_spTarget->Seek(offset, seekOrigin, &result), \
            StreamException, \
            "Cannot seek the file stream."); \
        return result; \
    } \

#define DECLARE_IOUTSTREAM_ADAPTER \
    UINT32 Write(std::vector<char> data) const; \
    UINT64 Seek(INT64 offset, UINT32 seekOrigin) const; \
    void SetSize(UINT64 newSize) const; \
    inline void SetTime( \
        const FILETIME *pftCreated, \
        const FILETIME *pftAccessed, \
        const FILETIME *pftModified) const \
    { \
        m_spTarget->SetTime(pftCreated, pftAccessed, pftModified); \
    } \
    inline UINT64 GetProcessedSize() const \
    { \
        return m_spTarget->GetProcessedSize(); \
    } \

#define IMPLEMENT_IOUTSTREAM_ADAPTER(target_name) \
    UINT32 target_name##Adapter::Write(std::vector<char> data) const \
    { \
        UINT32 result = 0; \
        CHECK_OK(m_spTarget->Write(data.data(), data.size(), &result), \
            StreamException, \
            "Cannot write to the file stream."); \
        return result; \
    } \
    UINT64 target_name##Adapter::Seek(INT64 offset, UINT32 seekOrigin) const \
    { \
        UINT64 result = 0; \
        CHECK_OK(m_spTarget->Seek(offset, seekOrigin, &result), \
            StreamException, \
            "Cannot seek the file stream."); \
        return result; \
    } \
    void target_name##Adapter::SetSize(UINT64 newSize) const \
    { \
        CHECK_OK(m_spTarget->SetSize(newSize), \
            StreamException, \
            "Cannot set size of the file stream."); \
    } \

#define DECLARE_ISTREAMGETSIZE_ADAPTER \
    UINT64 GetSize() const; \

#define IMPLEMENT_ISTREAMGETSIZE_ADAPTER(target_name) \
    UINT64 target_name##Adapter::GetSize() const \
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
