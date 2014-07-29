#ifndef SEVENZIPCORE_IARCHIVE_H
#define SEVENZIPCORE_IARCHIVE_H

#include <Unknwn.h>

#include "IDecl.h"
#include "IProgress.h"
#include "IStream.h"

namespace SevenZipCore
{
#define ARCHIVE_INTERFACE_SUB(i, base, x) DECL_INTERFACE_SUB(i, base, 6, x)
#define ARCHIVE_INTERFACE(i, x) ARCHIVE_INTERFACE_SUB(i, IUnknown, x)

    enum struct FileTimeType
    {
        Windows,
        Unix,
        DOS
    };

    // TODO: Modify to enum struct.
    enum struct FormatInfo
    {
        Name = 0,
        ClassID,
        Extension,
        AdditionExtension,
        Update,
        KeepName,
        StartSignature,
        FinishSignature,
        Associate
    };
    namespace NArchive
    {
        namespace NExtract
        {
            namespace NAskMode
            {
                enum
                {
                    Extract = 0,
                    Test,
                    Skip
                };
            }
            namespace NOperationResult
            {
                enum
                {
                    OK = 0,
                    UnSupportedMethod,
                    DataError,
                    CRCError
                };
            }
        }
        namespace NUpdate
        {
            namespace NOperationResult
            {
                enum
                {
                    OK = 0,
                    Error
                };
            }
        }
    }

#define INTERFACE_IArchiveOpenCallback(x) \
    STDMETHOD(SetTotal)(const UINT64 *files, const UINT64 *bytes) x; \
    STDMETHOD(SetCompleted)(const UINT64 *files, const UINT64 *bytes) x; \

    ARCHIVE_INTERFACE(IArchiveOpenCallback, 0x10)
    {
        INTERFACE_IArchiveOpenCallback(PURE);
    };


#define INTERFACE_IArchiveExtractCallback(x) \
    INTERFACE_IProgress(x) \
    STDMETHOD(GetStream)(UINT32 index, ISequentialOutStream **outStream, INT32 askExtractMode) x; \
    STDMETHOD(PrepareOperation)(INT32 askExtractMode) x; \
    STDMETHOD(SetOperationResult)(INT32 resultEOperationResult) x; \

    ARCHIVE_INTERFACE_SUB(IArchiveExtractCallback, IProgress, 0x20)
    {
        INTERFACE_IArchiveExtractCallback(PURE)
    };


#define INTERFACE_IArchiveOpenVolumeCallback(x) \
    STDMETHOD(GetProperty)(PROPID propID, PROPVARIANT *value) x; \
    STDMETHOD(GetStream)(const wchar_t *name, IInStream **inStream) x; \

    ARCHIVE_INTERFACE(IArchiveOpenVolumeCallback, 0x30)
    {
        INTERFACE_IArchiveOpenVolumeCallback(PURE);
    };


    ARCHIVE_INTERFACE(IInArchiveGetStream, 0x40)
    {
        STDMETHOD(GetStream)(UINT32 index, ISequentialInStream **stream) PURE;
    };


    ARCHIVE_INTERFACE(IArchiveOpenSetSubArchiveName, 0x50)
    {
        STDMETHOD(SetSubArchiveName)(const wchar_t *name) PURE;
    };


    /*
    IInArchive::Extract:
    indices must be sorted
    numItems = 0xFFFFFFFF means "all files"
    testMode != 0 means "test files without writing to outStream"
    */

#define INTERFACE_IInArchive(x) \
    STDMETHOD(Open)(IInStream *stream, const UINT64 *maxCheckStartPosition, IArchiveOpenCallback *openArchiveCallback) x; \
    STDMETHOD(Close)() x; \
    STDMETHOD(GetNumberOfItems)(UINT32 *numItems) x; \
    STDMETHOD(GetProperty)(UINT32 index, PROPID propID, PROPVARIANT *value) x; \
    STDMETHOD(Extract)(const UINT32* indices, UINT32 numItems, INT32 testMode, IArchiveExtractCallback *extractCallback) x; \
    STDMETHOD(GetArchiveProperty)(PROPID propID, PROPVARIANT *value) x; \
    STDMETHOD(GetNumberOfProperties)(UINT32 *numProperties) x; \
    STDMETHOD(GetPropertyInfo)(UINT32 index, BSTR *name, PROPID *propID, VARTYPE *varType) x; \
    STDMETHOD(GetNumberOfArchiveProperties)(UINT32 *numProperties) x; \
    STDMETHOD(GetArchivePropertyInfo)(UINT32 index, BSTR *name, PROPID *propID, VARTYPE *varType) x;

    ARCHIVE_INTERFACE(IInArchive, 0x60)
    {
        INTERFACE_IInArchive(PURE)
    };

    ARCHIVE_INTERFACE(IArchiveOpenSeq, 0x61)
    {
        STDMETHOD(OpenSeq)(ISequentialInStream *stream) PURE;
    };

#define INTERFACE_IArchiveUpdateCallback(x) \
    INTERFACE_IProgress(x); \
    STDMETHOD(GetUpdateItemInfo)(UINT32 index,  \
    INT32 *newData, /*1 - new data, 0 - old data */ \
    INT32 *newProperties, /* 1 - new properties, 0 - old properties */ \
    UINT32 *indexInArchive /* -1 if there is no in archive, or if doesn't matter */ \
    )  x; \
    STDMETHOD(GetProperty)(UINT32 index, PROPID propID, PROPVARIANT *value) x; \
    STDMETHOD(GetStream)(UINT32 index, ISequentialInStream **inStream) x; \
    STDMETHOD(SetOperationResult)(INT32 operationResult) x; \

    ARCHIVE_INTERFACE_SUB(IArchiveUpdateCallback, IProgress, 0x80)
    {
        INTERFACE_IArchiveUpdateCallback(PURE);
    };

#define INTERFACE_IArchiveUpdateCallback2(x) \
    INTERFACE_IArchiveUpdateCallback(x) \
    STDMETHOD(GetVolumeSize)(UINT32 index, UINT64 *size) x; \
    STDMETHOD(GetVolumeStream)(UINT32 index, ISequentialOutStream **volumeStream) x; \

    ARCHIVE_INTERFACE_SUB(IArchiveUpdateCallback2, IArchiveUpdateCallback, 0x82)
    {
        INTERFACE_IArchiveUpdateCallback2(PURE);
    };


#define INTERFACE_IOutArchive(x) \
    STDMETHOD(UpdateItems)(ISequentialOutStream *outStream, UINT32 numItems, IArchiveUpdateCallback *updateCallback) x; \
    STDMETHOD(GetFileTimeType)(UINT32 *type) x;

    ARCHIVE_INTERFACE(IOutArchive, 0xA0)
    {
        INTERFACE_IOutArchive(PURE)
    };


    ARCHIVE_INTERFACE(ISetProperties, 0x03)
    {
        STDMETHOD(SetProperties)(const wchar_t **names, const PROPVARIANT *values, INT32 numProperties) PURE;
    };


#define IMP_IInArchive_GetProp(k) \
    (UINT32 index, BSTR *name, PROPID *propID, VARTYPE *varType) \
    { if(index >= sizeof(k) / sizeof(k[0])) return E_INVALIDARG; \
    const STATPROPSTG &srcItem = k[index]; \
    *propID = srcItem.propid; *varType = srcItem.vt; *name = 0; return S_OK; } \

#define IMP_IInArchive_GetProp_WITH_NAME(k) \
    (UINT32 index, BSTR *name, PROPID *propID, VARTYPE *varType) \
    { if(index >= sizeof(k) / sizeof(k[0])) return E_INVALIDARG; \
    const STATPROPSTG &srcItem = k[index]; \
    *propID = srcItem.propid; *varType = srcItem.vt; \
    if (srcItem.lpwstrName == 0) *name = 0; else *name = ::SysAllocString(srcItem.lpwstrName); return S_OK; } \

#define IMP_IInArchive_Props \
    STDMETHODIMP CHandler::GetNumberOfProperties(UINT32 *numProperties) \
    { *numProperties = sizeof(kProps) / sizeof(kProps[0]); return S_OK; } \
    STDMETHODIMP CHandler::GetPropertyInfo IMP_IInArchive_GetProp(kProps)

#define IMP_IInArchive_Props_WITH_NAME \
    STDMETHODIMP CHandler::GetNumberOfProperties(UINT32 *numProperties) \
    { *numProperties = sizeof(kProps) / sizeof(kProps[0]); return S_OK; } \
    STDMETHODIMP CHandler::GetPropertyInfo IMP_IInArchive_GetProp_WITH_NAME(kProps)


#define IMP_IInArchive_ArcProps \
    STDMETHODIMP CHandler::GetNumberOfArchiveProperties(UINT32 *numProperties) \
    { *numProperties = sizeof(kArcProps) / sizeof(kArcProps[0]); return S_OK; } \
    STDMETHODIMP CHandler::GetArchivePropertyInfo IMP_IInArchive_GetProp(kArcProps)

#define IMP_IInArchive_ArcProps_WITH_NAME \
    STDMETHODIMP CHandler::GetNumberOfArchiveProperties(UINT32 *numProperties) \
    { *numProperties = sizeof(kArcProps) / sizeof(kArcProps[0]); return S_OK; } \
    STDMETHODIMP CHandler::GetArchivePropertyInfo IMP_IInArchive_GetProp_WITH_NAME(kArcProps)

#define IMP_IInArchive_ArcProps_NO_Table \
    STDMETHODIMP CHandler::GetNumberOfArchiveProperties(UINT32 *numProperties) \
    { *numProperties = 0; return S_OK; } \
    STDMETHODIMP CHandler::GetArchivePropertyInfo(UINT32, BSTR *, PROPID *, VARTYPE *) \
    { return E_NOTIMPL; } \

#define IMP_IInArchive_ArcProps_NO \
    IMP_IInArchive_ArcProps_NO_Table \
    STDMETHODIMP CHandler::GetArchiveProperty(PROPID, PROPVARIANT *value) \
    { value->vt = VT_EMPTY; return S_OK; }

}

#endif // SEVENZIPCORE_IARCHIVE_H
