// COM.h

#ifndef SEVENZIPCORE_COM_H
#define SEVENZIPCORE_COM_H

#include <Windows.h>

#ifdef _DEBUG
#include "TString.h"
#endif

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

#ifdef _DEBUG
#define ADDREF_RELEASE \
    public: \
    STDMETHOD_(ULONG, AddRef)() \
    { \
        TStringStream tss; \
        tss << TEXT("AddRef: 0x") << std::hex << this; \
        tss << " (" << m_ulRefCount << " -> " << (m_ulRefCount + 1) << ")" << TEXT('\n'); \
        OutputDebugString(tss.str().c_str()); \
        return ++m_ulRefCount; \
    } \
    STDMETHOD_(ULONG, Release)() \
    { \
        TStringStream tss; \
        tss << TEXT("Release: 0x") << std::hex << this; \
        tss << " (" << m_ulRefCount << " -> " << (m_ulRefCount - 1) << ")" << TEXT('\n'); \
        OutputDebugString(tss.str().c_str()); \
        if (--m_ulRefCount) \
        { \
            return m_ulRefCount; \
        } \
        delete this; \
        return 0; \
    } \
    private: \
    ULONG m_ulRefCount = 0;
#else
#define ADDREF_RELEASE \
    public: \
    STDMETHOD_(ULONG, AddRef)() \
    { \
        return ++m_ulRefCount; \
    } \
    STDMETHOD_(ULONG, Release)() \
    { \
        if (--m_ulRefCount) \
        { \
            return m_ulRefCount; \
        } \
        delete this; \
        return 0; \
    } \
    private: \
    ULONG m_ulRefCount = 0;
#endif

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

#endif // SEVENZIPCORE_COM_H