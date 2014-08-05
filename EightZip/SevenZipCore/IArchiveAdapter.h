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
        IArchiveOpenCallback *openArchiveCallback) const; \
    void Close() const; \
    UINT32 GetNumberOfItems() const; \
    PROPVARIANT GetProperty(UINT32 index, PropertyId propID) const; \
    void Extract( \
        const std::vector<UINT32> &indices, \
        INT32 testMode, \
        IArchiveExtractCallback *extractCallback) const; \
    PROPVARIANT GetArchiveProperty(PropertyId propID) const; \
    UINT32 GetNumberOfProperties() const; \
    void GetPropertyInfo( \
        UINT32 index, \
        BSTR *name, \
        PROPID *propID, \
        VARTYPE *varType) const; \
    UINT32 GetNumberOfArchiveProperties() const; \
    void GetArchivePropertyInfo( \
        UINT32 index, \
        BSTR *name, \
        PROPID *propID, \
        VARTYPE *varType) const; \
    TString GetItemPath(UINT32 index) const;

#define IMPLEMENT_IINARCHIVE_ADAPTER(target_name) \
    void target_name##Adapter::Open( \
        IInStream *stream, \
        UINT64 maxCheckStartPosition, \
        IArchiveOpenCallback *openArchiveCallback) const \
    { \
        CHECK_OK(m_spTarget->Open( \
            stream, &maxCheckStartPosition, openArchiveCallback), \
            ArchiveException, "Cannot open the specified stream."); \
    } \
    void target_name##Adapter::Close() const \
    { \
        CHECK_OK(m_spTarget->Close(), \
            ArchiveException, \
            "Cannot close the archive."); \
    } \
    UINT32 target_name##Adapter::GetNumberOfItems() const \
    { \
        UINT32 result = 0; \
        CHECK_OK(m_spTarget->GetNumberOfItems(&result), \
            ArchiveException, \
            "Cannot get number of archive items"); \
        return result; \
    } \
    PROPVARIANT target_name##Adapter::GetProperty( \
        UINT32 index, \
        PropertyId propertyID) const \
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
        IArchiveExtractCallback *extractCallback) const \
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
        PropertyId propertyId) const \
    { \
        PROPVARIANT result = {}; \
        CHECK_OK(m_spTarget->GetArchiveProperty( \
            static_cast<PROPID>(propertyId), \
            &result), \
            ArchiveException, \
            "Cannot get property of the archive."); \
        return result; \
    } \
    UINT32 target_name##Adapter::GetNumberOfProperties() const \
    { \
        UINT32 result = 0; \
        CHECK_OK(m_spTarget->GetNumberOfProperties(&result), \
            ArchiveException, \
            "Cannot get number of archive properties"); \
        return result; \
    } \
    void target_name##Adapter::GetPropertyInfo( \
        UINT32 index, BSTR *name, PROPID *propID, VARTYPE *varType) const \
    { \
        CHECK_OK(m_spTarget->GetPropertyInfo( \
            index, name, propID, varType), \
            ArchiveException, \
            "Cannot get property info of the archive."); \
    } \
    UINT32 target_name##Adapter::GetNumberOfArchiveProperties() const \
    { \
        UINT32 result = 0; \
        CHECK_OK(m_spTarget->GetNumberOfArchiveProperties(&result), \
            ArchiveException, "Cannot get number of archive properties."); \
        return result; \
    } \
    void target_name##Adapter::GetArchivePropertyInfo( \
        UINT32 index, BSTR *name, PROPID *propID, VARTYPE *varType) const \
    { \
        CHECK_OK(m_spTarget->GetArchivePropertyInfo( \
            index, name, propID, varType), \
            ArchiveException, \
            "Cannot get property info of the file in archive."); \
    } \
    TString target_name##Adapter::GetItemPath(UINT32 index) const \
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
