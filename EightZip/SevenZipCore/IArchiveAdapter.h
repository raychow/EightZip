// IArchiveWrapeer.h

#ifndef SEVENZIPCORE_IARCHIVEWRAPEER_H
#define SEVENZIPCORE_IARCHIVEWRAPEER_H

#include <memory>
#include <vector>

#include "Adapter.h"
#include "IArchive.h"
#include "Property.h"
#include "TString.h"

#define DECLARE_IINARCHIVE_ADAPTER \
    void Open( \
        IInStream *stream, \
        UINT64 maxCheckStartPosition, \
        IArchiveOpenCallback *openArchiveCallback); \
    void Close(); \
    UINT32 GetNumberOfItems(); \
    PROPVARIANT GetProperty(UINT32 index, PropertyId propID); \
    void Extract( \
        const std::vector<UINT32> &indices, \
        INT32 testMode, \
        IArchiveExtractCallback *extractCallback); \
    PROPVARIANT GetArchiveProperty(PropertyId propID); \
    UINT32 GetNumberOfProperties(); \
    void GetPropertyInfo( \
        UINT32 index, \
        BSTR *name, \
        PROPID *propID, \
        VARTYPE *varType); \
    UINT32 GetNumberOfArchiveProperties(); \
    void GetArchivePropertyInfo( \
        UINT32 index, \
        BSTR *name, \
        PROPID *propID, \
        VARTYPE *varType); \
    TString GetItemPath(UINT32 index);

#define IMPLEMENT_IINARCHIVE_ADAPTER(target_name) \
    void target_name##Adapter::Open( \
        IInStream *stream, \
        UINT64 maxCheckStartPosition, \
        IArchiveOpenCallback *openArchiveCallback) \
    { \
        CHECK_OK(m_spTarget->Open( \
            stream, &maxCheckStartPosition, openArchiveCallback), \
            ArchiveException, "Cannot open the specified stream."); \
    } \
    void target_name##Adapter::Close() \
    { \
        CHECK_OK(m_spTarget->Close(), \
            ArchiveException, \
            "Cannot close the archive."); \
    } \
    UINT32 target_name##Adapter::GetNumberOfItems() \
    { \
        UINT32 result = 0; \
        CHECK_OK(m_spTarget->GetNumberOfItems(&result), \
            ArchiveException, \
            "Cannot get number of archive items"); \
        return result; \
    } \
    PROPVARIANT target_name##Adapter::GetProperty( \
        UINT32 index, \
        PropertyId propertyID) \
    { \
        PROPVARIANT result = {}; \
        CHECK_OK(m_spTarget->GetProperty( \
            index, \
            static_cast<PROPID>(propertyID), &result), \
            ArchiveException, \
            "Cannot get property of the file in archive."); \
        return result; \
    } \
    void target_name##Adapter::Extract( \
        const std::vector<UINT32> &indices, \
        INT32 testMode, \
        IArchiveExtractCallback *extractCallback) \
    { \
        CHECK_OK(m_spTarget->Extract( \
            indices.data(), \
            indices.size(), \
            testMode, \
            extractCallback), \
            ArchiveException, \
            "Can not extract files from the archive."); \
    } \
    PROPVARIANT target_name##Adapter::GetArchiveProperty( \
        PropertyId propertyId) \
    { \
        PROPVARIANT result = {}; \
        CHECK_OK(m_spTarget->GetArchiveProperty( \
            static_cast<PROPID>(propertyId), \
            &result), \
            ArchiveException, \
            "Cannot get property of the archive."); \
        return result; \
    } \
    UINT32 target_name##Adapter::GetNumberOfProperties() \
    { \
        UINT32 result = 0; \
        CHECK_OK(m_spTarget->GetNumberOfProperties(&result), \
            ArchiveException, \
            "Cannot get number of archive properties"); \
        return result; \
    } \
    void target_name##Adapter::GetPropertyInfo( \
        UINT32 index, BSTR *name, PROPID *propID, VARTYPE *varType) \
    { \
        CHECK_OK(m_spTarget->GetPropertyInfo( \
            index, name, propID, varType), \
            ArchiveException, \
            "Cannot get property info of the archive."); \
    } \
    UINT32 target_name##Adapter::GetNumberOfArchiveProperties() \
    { \
        UINT32 result = 0; \
        CHECK_OK(m_spTarget->GetNumberOfArchiveProperties(&result), \
            ArchiveException, "Cannot get number of archive properties."); \
        return result; \
    } \
    void target_name##Adapter::GetArchivePropertyInfo( \
        UINT32 index, BSTR *name, PROPID *propID, VARTYPE *varType) \
    { \
        CHECK_OK(m_spTarget->GetArchivePropertyInfo( \
            index, name, propID, varType), \
            ArchiveException, \
            "Cannot get property info of the file in archive."); \
    } \
    TString target_name##Adapter::GetItemPath(UINT32 index) \
    { \
        auto tstrPath = PropertyHelper::GetString( \
            GetProperty(index, PropertyId::Path), TString()); \
        if (tstrPath.empty()) \
        { \
            auto tstrExtension = PropertyHelper::GetString( \
                GetProperty(index, PropertyId::Extension), TString()); \
            if (!tstrExtension.empty()) \
            { \
                tstrPath = TString(TEXT(".")) + tstrExtension; \
            } \
        } \
        return tstrPath; \
    } \

namespace SevenZipCore
{
    DECLARE_ADAPTER_CLASS1(IInArchive, DECLARE_IINARCHIVE_ADAPTER)
}

#endif // SEVENZIPCORE_IARCHIVEWRAPEER_H
