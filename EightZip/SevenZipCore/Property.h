// Property.h

#ifndef SEVENZIPCORE_PROPERTY_H
#define SEVENZIPCORE_PROPERTY_H

#include <vector>

#include "Platform.h"

#include "TString.h"

namespace SevenZipCore
{
    enum class PropertyId
    {
        NoProperty = 0,
        MainSubfile = 1,
        HandlerItemIndex = 2,
        Path,
        Name,
        Extension,
        IsDir,
        Size,
        PackSize,
        Attribute,
        Created,
        Accessed,
        Modified,
        Solid,
        Commented,
        Encrypted,
        SplitBefore,
        SplitAfter,
        DictionarySize,
        CRC,
        Type,
        IsAnti,
        Method,
        HostOS,
        FileSystem,
        User,
        Group,
        Block,
        Comment,
        Position,
        Prefix,
        NumSubDirs,
        NumSubFiles,
        UnpackVer,
        Volume,
        IsVolume,
        Offset,
        Links,
        NumBlocks,
        NumVolumes,
        TimeType,
        Bit64,
        BigEndian,
        Cpu,
        PhySize,
        HeadersSize,
        Checksum,
        Characts,
        Va,
        Id,
        ShortName,
        CreatorApp,
        SectorSize,
        PosixAttrib,
        Link,
        Error,

        TotalSize = 0x1100,
        FreeSpace,
        ClusterSize,
        VolumeName,

        LocalName = 0x1200,
        Provider,

        UserDefined = 0x10000
    };

#define DECLARE_PROPERTY_HELPER(type, name) \
    static type name(const PROPVARIANT &property); \
    static type name(const PROPVARIANT &property, type defaultValue);

#define IMPLEMENT_PROPERTY_HELPER_WITH_DEFAULT_VALUE(type, name) \
    type PropertyHelper::name(const PROPVARIANT &property, type defaultValue) \
    { \
        try \
        { \
            return name(property); \
        } \
        catch (const PropertyException &ex) \
        { \
            if (PropertyErrorCode::EMPTY_VALUE == ex.GetErrorCode()) \
            { \
                return defaultValue; \
            } \
            throw; \
        } \
    }

    class PropertyHelper
    {
    public:
        DECLARE_PROPERTY_HELPER(bool, GetBool)
        DECLARE_PROPERTY_HELPER(BYTE, GetByte)
        DECLARE_PROPERTY_HELPER(USHORT, GetUShort)
        DECLARE_PROPERTY_HELPER(UINT32, GetUInt32)
        DECLARE_PROPERTY_HELPER(UINT64, GetUInt64)
        DECLARE_PROPERTY_HELPER(TString, GetString)
        DECLARE_PROPERTY_HELPER(GUID, GetGUID)
        DECLARE_PROPERTY_HELPER(std::vector<BYTE>, GetBytes)
        DECLARE_PROPERTY_HELPER(UINT64, GetConvertedUInt64)
        DECLARE_PROPERTY_HELPER(FILETIME, GetFileTime)
    };
}

#endif // SEVENZIPCORE_PROPERTY_H