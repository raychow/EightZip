#include "stdwx.h"
#include "Property.h"

#include <Windows.h>

#include "Exception.h"

using namespace std;

namespace SevenZipCore
{
    bool PropertyHelper::GetBool(const PROPVARIANT &property)
    {
        switch (property.vt)
        {
        case VT_BOOL:
            return property.boolVal != VARIANT_FALSE;
        case VT_EMPTY:
            return false;
        default:
            throw PropertyException("Get a non-bool value when read bool property.");
        }
    }

    TString PropertyHelper::GetString(const PROPVARIANT &property)
    {
        switch (property.vt)
        {
        case VT_BSTR:
            return property.bstrVal;
        case VT_EMPTY:
            return TEXT("");
        default:
            throw PropertyException("Get a non-bstr value when read string property.");
        }
    }

    GUID PropertyHelper::GetGUID(const PROPVARIANT &property)
    {
        switch (property.vt)
        {
        case VT_BSTR:
            return *(const GUID*)property.bstrVal;
        default:
            throw PropertyException("Get a non-bstr value when read guid property.");
        }
    }

    vector<BYTE> PropertyHelper::GetBytes(const PROPVARIANT &property)
    {
        vector<BYTE> result;
        switch (property.vt)
        {
        case VT_BSTR:
            result.assign(reinterpret_cast<BYTE *>(property.bstrVal), reinterpret_cast<BYTE *>(property.bstrVal) + ::SysStringByteLen(property.bstrVal));
            break;
        case VT_EMPTY:
            result.clear();
            break;
        default:
            throw PropertyException("Get a non-bstr value when read bytes property.");
        }
        return result;
    }

    BYTE PropertyHelper::GetByte(const PROPVARIANT &property)
    {
        switch (property.vt)
        {
        case VT_UI1:
            return property.bVal;
        case VT_EMPTY:
            return 0;
        default:
            throw PropertyException("Get a non-ui1 value when read byte property.");
        }
    }

    USHORT PropertyHelper::GetUShort(const PROPVARIANT &property)
    {
        switch (property.vt)
        {
        case VT_UI2:
            return property.uiVal;
        case VT_EMPTY:
            return 0;
        default:
            throw PropertyException("Get a non-ui2 value when read ushort property.");
        }
    }

    UINT32 PropertyHelper::GetUInt32(const PROPVARIANT &property)
    {
        switch (property.vt)
        {
        case VT_UI4:
            return property.ulVal;
        case VT_EMPTY:
            return 0;
        default:
            throw PropertyException("Get a non-ui4 value when read uint32 property.");
        }
    }

    UINT64 PropertyHelper::GetUInt64(const PROPVARIANT &property)
    {
        switch (property.vt)
        {
        case VT_UI8:
            return property.uhVal.QuadPart;
        case VT_EMPTY:
            return 0;
        default:
            throw PropertyException("Get a non-ui8 value when read uint64 property.");
        }
    }

    UINT64 PropertyHelper::GetConvertedUInt64(const PROPVARIANT &property)
    {
        switch (property.vt)
        {
        case VT_UI1:
            return property.bVal;
        case VT_UI2:
            return property.uiVal;
        case VT_UI4:
            return property.ulVal;
        case VT_UI8:
            return property.uhVal.QuadPart;
        case VT_EMPTY:
            return 0;
        default:
            throw PropertyException("Get a non-unsigned value when read converted uint64 property.");
        }
    }

}
