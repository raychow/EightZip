#include "PropertyVariant.h"

namespace SevenZipCore
{
    PropertyVariant::PropertyVariant(const PROPVARIANT &varSrc)
    {
        vt = VT_EMPTY;
        InternalCopy(&varSrc);
    }

    PropertyVariant::PropertyVariant(const PropertyVariant &varSrc)
    {
        vt = VT_EMPTY;
        InternalCopy(&varSrc);
    }

    PropertyVariant::PropertyVariant(BSTR bstrSrc)
    {
        vt = VT_EMPTY;
        *this = bstrSrc;
    }

    PropertyVariant::PropertyVariant(LPCOLESTR lpszSrc)
    {
        vt = VT_EMPTY;
        *this = lpszSrc;
    }

    PropertyVariant& PropertyVariant::operator=(const PropertyVariant &varSrc)
    {
        InternalCopy(&varSrc);
        return *this;
    }
    PropertyVariant& PropertyVariant::operator=(const PROPVARIANT &varSrc)
    {
        InternalCopy(&varSrc);
        return *this;
    }

    PropertyVariant& PropertyVariant::operator=(BSTR bstrSrc)
    {
        *this = (LPCOLESTR)bstrSrc;
        return *this;
    }

    static const char *kMemException = "out of memory";

    PropertyVariant& PropertyVariant::operator=(LPCOLESTR lpszSrc)
    {
        InternalClear();
        vt = VT_BSTR;
        wReserved1 = 0;
        bstrVal = ::SysAllocString(lpszSrc);
        if (bstrVal == NULL && lpszSrc != NULL)
        {
            throw kMemException;
            // vt = VT_ERROR;
            // scode = E_OUTOFMEMORY;
        }
        return *this;
    }


    PropertyVariant& PropertyVariant::operator=(const char *s)
    {
        InternalClear();
        vt = VT_BSTR;
        wReserved1 = 0;
        UINT len = (UINT)strlen(s);
        bstrVal = ::SysAllocStringByteLen(0, (UINT)len * sizeof(OLECHAR));
        if (bstrVal == NULL)
        {
            throw kMemException;
            // vt = VT_ERROR;
            // scode = E_OUTOFMEMORY;
        }
        else
        {
            for (UINT i = 0; i <= len; i++)
                bstrVal[i] = s[i];
        }
        return *this;
    }

    PropertyVariant& PropertyVariant::operator=(bool bSrc)
    {
        if (vt != VT_BOOL)
        {
            InternalClear();
            vt = VT_BOOL;
        }
        boolVal = bSrc ? VARIANT_TRUE : VARIANT_FALSE;
        return *this;
    }

#define SET_PROP_FUNC(type, id, dest) \
    PropertyVariant& PropertyVariant::operator=(type value) \
    { if (vt != id) { InternalClear(); vt = id; } \
    dest = value; return *this; }

    SET_PROP_FUNC(BYTE, VT_UI1, bVal)
        SET_PROP_FUNC(INT16, VT_I2, iVal)
        SET_PROP_FUNC(INT32, VT_I4, lVal)
        SET_PROP_FUNC(UINT32, VT_UI4, ulVal)
        SET_PROP_FUNC(UINT64, VT_UI8, uhVal.QuadPart)
        SET_PROP_FUNC(const FILETIME &, VT_FILETIME, filetime)

        static HRESULT MyPropVariantClear(PROPVARIANT *prop)
    {
            switch (prop->vt)
            {
            case VT_UI1:
            case VT_I1:
            case VT_I2:
            case VT_UI2:
            case VT_BOOL:
            case VT_I4:
            case VT_UI4:
            case VT_R4:
            case VT_INT:
            case VT_UINT:
            case VT_ERROR:
            case VT_FILETIME:
            case VT_UI8:
            case VT_R8:
            case VT_CY:
            case VT_DATE:
                prop->vt = VT_EMPTY;
                prop->wReserved1 = 0;
                return S_OK;
            }
            return ::VariantClear((VARIANTARG *)prop);
    }

    HRESULT PropertyVariant::Clear()
    {
        return MyPropVariantClear(this);
    }

    HRESULT PropertyVariant::Copy(const PROPVARIANT* pSrc)
    {
        ::VariantClear((tagVARIANT *)this);
        switch (pSrc->vt)
        {
        case VT_UI1:
        case VT_I1:
        case VT_I2:
        case VT_UI2:
        case VT_BOOL:
        case VT_I4:
        case VT_UI4:
        case VT_R4:
        case VT_INT:
        case VT_UINT:
        case VT_ERROR:
        case VT_FILETIME:
        case VT_UI8:
        case VT_R8:
        case VT_CY:
        case VT_DATE:
            memmove((PROPVARIANT*)this, pSrc, sizeof(PROPVARIANT));
            return S_OK;
        }
        return ::VariantCopy(
            (tagVARIANT *)this, (tagVARIANT *)const_cast<PROPVARIANT *>(pSrc));
    }


    HRESULT PropertyVariant::Attach(PROPVARIANT *pSrc)
    {
        HRESULT hr = Clear();
        if (FAILED(hr))
            return hr;
        memcpy(this, pSrc, sizeof(PROPVARIANT));
        pSrc->vt = VT_EMPTY;
        return S_OK;
    }

    HRESULT PropertyVariant::Detach(PROPVARIANT *pDest)
    {
        HRESULT hr = MyPropVariantClear(pDest);
        if (FAILED(hr))
            return hr;
        memcpy(pDest, this, sizeof(PROPVARIANT));
        vt = VT_EMPTY;
        return S_OK;
    }

    HRESULT PropertyVariant::InternalClear()
    {
        HRESULT hr = Clear();
        if (FAILED(hr))
        {
            vt = VT_ERROR;
            scode = hr;
        }
        return hr;
    }

    void PropertyVariant::InternalCopy(const PROPVARIANT *pSrc)
    {
        HRESULT hr = Copy(pSrc);
        if (FAILED(hr))
        {
            if (hr == E_OUTOFMEMORY)
                throw kMemException;
            vt = VT_ERROR;
            scode = hr;
        }
    }

    //int PropertyVariant::Compare(const PropertyVariant &a)
    //{
    //    if (vt != a.vt)
    //        return MyCompare(vt, a.vt);
    //    switch (vt)
    //    {
    //    case VT_EMPTY: return 0;
    //        // case VT_I1: return MyCompare(cVal, a.cVal);
    //    case VT_UI1: return MyCompare(bVal, a.bVal);
    //    case VT_I2: return MyCompare(iVal, a.iVal);
    //    case VT_UI2: return MyCompare(uiVal, a.uiVal);
    //    case VT_I4: return MyCompare(lVal, a.lVal);
    //    case VT_UI4: return MyCompare(ulVal, a.ulVal);
    //        // case VT_UINT: return MyCompare(uintVal, a.uintVal);
    //    case VT_I8: return MyCompare(hVal.QuadPart, a.hVal.QuadPart);
    //    case VT_UI8: return MyCompare(uhVal.QuadPart, a.uhVal.QuadPart);
    //    case VT_BOOL: return -MyCompare(boolVal, a.boolVal);
    //    case VT_FILETIME: return ::CompareFileTime(&filetime, &a.filetime);
    //    case VT_BSTR:
    //        return 0; // Not implemented
    //        // return MyCompare(aPropVarint.cVal);
    //    default: return 0;
    //    }
    //}
}
