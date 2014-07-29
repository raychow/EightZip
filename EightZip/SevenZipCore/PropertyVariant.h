// PropertyVariant.h

#ifndef SEVENZIPCORE_PROPERTYVARIANT_H
#define SEVENZIPCORE_PROPERTYVARIANT_H

#include <Windows.h>

namespace SevenZipCore
{
    class PropertyVariant
        : public PROPVARIANT
    {
    public:
        PropertyVariant() { vt = VT_EMPTY; wReserved1 = 0; }
        ~PropertyVariant() { Clear(); }
        PropertyVariant(const PROPVARIANT &varSrc);
        PropertyVariant(const PropertyVariant &varSrc);
        PropertyVariant(BSTR bstrSrc);
        PropertyVariant(LPCOLESTR lpszSrc);
        PropertyVariant(bool bSrc) { vt = VT_BOOL; wReserved1 = 0; boolVal = (bSrc ? VARIANT_TRUE : VARIANT_FALSE); }
        PropertyVariant(BYTE value) { vt = VT_UI1; wReserved1 = 0; bVal = value; }
        PropertyVariant(INT16 value) { vt = VT_I2; wReserved1 = 0; iVal = value; }
        PropertyVariant(INT32 value) { vt = VT_I4; wReserved1 = 0; lVal = value; }
        PropertyVariant(UINT32 value) { vt = VT_UI4; wReserved1 = 0; ulVal = value; }
        PropertyVariant(UINT64 value) { vt = VT_UI8; wReserved1 = 0; uhVal.QuadPart = value; }
        PropertyVariant(const FILETIME &value) { vt = VT_FILETIME; wReserved1 = 0; filetime = value; }

        PropertyVariant& operator=(const PropertyVariant &varSrc);
        PropertyVariant& operator=(const PROPVARIANT &varSrc);
        PropertyVariant& operator=(BSTR bstrSrc);
        PropertyVariant& operator=(LPCOLESTR lpszSrc);
        PropertyVariant& operator=(const CHAR *s);
        PropertyVariant& operator=(bool bSrc);
        PropertyVariant& operator=(BYTE value);
        PropertyVariant& operator=(INT16 value);
        PropertyVariant& operator=(INT32 value);
        PropertyVariant& operator=(UINT32 value);
        PropertyVariant& operator=(INT64 value);
        PropertyVariant& operator=(UINT64 value);
        PropertyVariant& operator=(const FILETIME &value);

        HRESULT Clear();
        HRESULT Copy(const PROPVARIANT *pSrc);
        HRESULT Attach(PROPVARIANT *pSrc);
        HRESULT Detach(PROPVARIANT *pDest);

        HRESULT InternalClear();
        void InternalCopy(const PROPVARIANT *pSrc);

        //int Compare(const PropertyVariant &a1);
    };
}

#endif // SEVENZIPCORE_PROPERTYVARIANT_H
