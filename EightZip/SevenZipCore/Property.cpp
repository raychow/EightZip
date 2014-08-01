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
            throw PropertyException(PropertyErrorCode::EMPTY_VALUE, "Get a empty value when read property.");
        default:
            throw PropertyException(PropertyErrorCode::INVALID_VALUE_TYPE, "Get a non-bool value when read bool property.");
        }
    }

    IMPLEMENT_PROPERTY_HELPER_WITH_DEFAULT_VALUE(bool, GetBool)

    TString PropertyHelper::GetString(const PROPVARIANT &property)
    {
        switch (property.vt)
        {
        case VT_BSTR:
            return property.bstrVal;
        case VT_EMPTY:
            throw PropertyException(PropertyErrorCode::EMPTY_VALUE, "Get a empty value when read property.");
        default:
            throw PropertyException(PropertyErrorCode::INVALID_VALUE_TYPE, "Get a non-bstr value when read string property.");
        }
    }

    IMPLEMENT_PROPERTY_HELPER_WITH_DEFAULT_VALUE(TString, GetString)

    GUID PropertyHelper::GetGUID(const PROPVARIANT &property)
    {
        switch (property.vt)
        {
        case VT_BSTR:
            return *(const GUID*)property.bstrVal;
        case VT_EMPTY:
            throw PropertyException(PropertyErrorCode::EMPTY_VALUE, "Get a empty value when read property.");
        default:
            throw PropertyException(PropertyErrorCode::INVALID_VALUE_TYPE, "Get a non-bstr value when read guid property.");
        }
    }

    IMPLEMENT_PROPERTY_HELPER_WITH_DEFAULT_VALUE(GUID, GetGUID)

    vector<BYTE> PropertyHelper::GetBytes(const PROPVARIANT &property)
    {
        vector<BYTE> result;
        switch (property.vt)
        {
        case VT_BSTR:
            result.assign(reinterpret_cast<BYTE *>(property.bstrVal), reinterpret_cast<BYTE *>(property.bstrVal) + ::SysStringByteLen(property.bstrVal));
            break;
        case VT_EMPTY:
            throw PropertyException(PropertyErrorCode::EMPTY_VALUE, "Get a empty value when read property.");
        default:
            throw PropertyException(PropertyErrorCode::INVALID_VALUE_TYPE, "Get a non-bstr value when read bytes property.");
        }
        return result;
    }

    IMPLEMENT_PROPERTY_HELPER_WITH_DEFAULT_VALUE(std::vector<BYTE>, GetBytes)

    BYTE PropertyHelper::GetByte(const PROPVARIANT &property)
    {
        switch (property.vt)
        {
        case VT_UI1:
            return property.bVal;
        case VT_EMPTY:
            throw PropertyException(PropertyErrorCode::EMPTY_VALUE, "Get a empty value when read property.");
        default:
            throw PropertyException(PropertyErrorCode::INVALID_VALUE_TYPE, "Get a non-ui1 value when read byte property.");
        }
    }

    IMPLEMENT_PROPERTY_HELPER_WITH_DEFAULT_VALUE(BYTE, GetByte)

    USHORT PropertyHelper::GetUShort(const PROPVARIANT &property)
    {
        switch (property.vt)
        {
        case VT_UI2:
            return property.uiVal;
        case VT_EMPTY:
            throw PropertyException(PropertyErrorCode::EMPTY_VALUE, "Get a empty value when read property.");
        default:
            throw PropertyException(PropertyErrorCode::INVALID_VALUE_TYPE, "Get a non-ui2 value when read ushort property.");
        }
    }

    IMPLEMENT_PROPERTY_HELPER_WITH_DEFAULT_VALUE(USHORT, GetUShort)

    UINT32 PropertyHelper::GetUInt32(const PROPVARIANT &property)
    {
        switch (property.vt)
        {
        case VT_UI4:
            return property.ulVal;
        case VT_EMPTY:
            throw PropertyException(PropertyErrorCode::EMPTY_VALUE, "Get a empty value when read property.");
        default:
            throw PropertyException(PropertyErrorCode::INVALID_VALUE_TYPE, "Get a non-ui4 value when read uint32 property.");
        }
    }

    IMPLEMENT_PROPERTY_HELPER_WITH_DEFAULT_VALUE(UINT32, GetUInt32)

    UINT64 PropertyHelper::GetUInt64(const PROPVARIANT &property)
    {
        switch (property.vt)
        {
        case VT_UI8:
            return property.uhVal.QuadPart;
        case VT_EMPTY:
            throw PropertyException(PropertyErrorCode::EMPTY_VALUE, "Get a empty value when read property.");
        default:
            throw PropertyException(PropertyErrorCode::INVALID_VALUE_TYPE, "Get a non-ui8 value when read uint64 property.");
        }
    }

    IMPLEMENT_PROPERTY_HELPER_WITH_DEFAULT_VALUE(UINT64, GetUInt64)

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
            throw PropertyException(PropertyErrorCode::EMPTY_VALUE, "Get a empty value when read property.");
        default:
            throw PropertyException(PropertyErrorCode::INVALID_VALUE_TYPE, "Get a non-unsigned value when read converted uint64 property.");
        }
    }

    IMPLEMENT_PROPERTY_HELPER_WITH_DEFAULT_VALUE(UINT64, GetConvertedUInt64)

    FILETIME PropertyHelper::GetFileTime(const PROPVARIANT &property)
    {
        switch (property.vt)
        {
        case VT_FILETIME:
            if (property.filetime.dwHighDateTime != 0 || property.filetime.dwLowDateTime != 0)
            {
                return property.filetime;
            }
            throw PropertyException(PropertyErrorCode::EMPTY_VALUE, "Get a empty value when read property.");
        case VT_EMPTY:
            throw PropertyException(PropertyErrorCode::EMPTY_VALUE, "Get a empty value when read property.");
        default:
            throw PropertyException(PropertyErrorCode::INVALID_VALUE_TYPE, "Get a non-filetime value when read filetime property.");
        }
    }

    IMPLEMENT_PROPERTY_HELPER_WITH_DEFAULT_VALUE(FILETIME, GetFileTime)
}
