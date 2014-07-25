// COM.h

#ifndef COM_H
#define COM_H

#include <Windows.h>

#define QUERYINTERFACE_BEGIN \
    STDMETHOD(QueryInterface)(REFGUID iid, void **outObject) \
    {

#define QUERYINTERFACE_ENTRY_UNKNOWN(i) \
    if (iid == IID_IUnknown) \
        { \
        *outObject = (void *)(IUnknown *)(i *)this; \
        AddRef(); \
        return S_OK; \
        }

#define QUERYINTERFACE_ENTRY(i) \
    if (iid == IID_ ## i) \
    { \
        *outObject = (void *)(i *)this; \
        AddRef(); \
        return S_OK; \
    }

#define QUERYINTERFACE_END \
        return E_NOINTERFACE; \
    }

#define IUNKNOWN_IMP_SPEC(i) \
    QUERYINTERFACE_BEGIN \
        i \
    QUERYINTERFACE_END \
    ADDREF_RELEASE

#define ADDREF_RELEASE \
    public: \
    STDMETHOD_(ULONG, AddRef)() { return ++m_ulRefCount; } \
    STDMETHOD_(ULONG, Release)() { if (--m_ulRefCount) { return m_ulRefCount; } delete this; return 0; } \
    private: \
    ULONG m_ulRefCount = 0;

#define IUNKNOWN_IMP \
    QUERYINTERFACE_BEGIN \
        QUERYINTERFACE_ENTRY_UNKNOWN(IUnknown) \
    QUERYINTERFACE_END \
    ADDREF_RELEASE

#define IUNKNOWN_IMP1(i) \
    IUNKNOWN_IMP_SPEC( \
        QUERYINTERFACE_ENTRY_UNKNOWN(i) \
        QUERYINTERFACE_ENTRY(i) \
    )

#define IUNKNOWN_IMP2(i1, i2) \
    IUNKNOWN_IMP_SPEC( \
        QUERYINTERFACE_ENTRY_UNKNOWN(i1) \
        QUERYINTERFACE_ENTRY(i1) \
        QUERYINTERFACE_ENTRY(i2) \
    )

#define IUNKNOWN_IMP3(i1, i2, i3) \
    IUNKNOWN_IMP_SPEC( \
        QUERYINTERFACE_ENTRY_UNKNOWN(i1) \
        QUERYINTERFACE_ENTRY(i1) \
        QUERYINTERFACE_ENTRY(i2) \
        QUERYINTERFACE_ENTRY(i3) \
    )

#define IUNKNOWN_IMP4(i1, i2, i3, i4) \
    IUNKNOWN_IMP_SPEC( \
        QUERYINTERFACE_ENTRY_UNKNOWN(i1) \
        QUERYINTERFACE_ENTRY(i1) \
        QUERYINTERFACE_ENTRY(i2) \
        QUERYINTERFACE_ENTRY(i3) \
        QUERYINTERFACE_ENTRY(i4) \
    )

#define IUNKNOWN_IMP5(i1, i2, i3, i4, i5) \
    IUNKNOWN_IMP_SPEC( \
        QUERYINTERFACE_ENTRY_UNKNOWN(i1) \
        QUERYINTERFACE_ENTRY(i1) \
        QUERYINTERFACE_ENTRY(i2) \
        QUERYINTERFACE_ENTRY(i3) \
        QUERYINTERFACE_ENTRY(i4) \
        QUERYINTERFACE_ENTRY(i5) \
    )

#endif // COM_H