// PropId.h

#ifndef PROPID_H
#define PROPID_H

#include <vector>

#include <Windows.h>

#include "TString.h"

namespace SevenZipCore
{
    enum class PropId
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
        Attrib,
        CTime,
        ATime,
        MTime,
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

    class PropertyHelper
    {
    public:
        static bool GetBool(const PROPVARIANT &property);
        static BYTE GetByte(const PROPVARIANT &property);
        static USHORT GetUShort(const PROPVARIANT &property);
        static UINT32 GetUInt32(const PROPVARIANT &property);
        static UINT64 GetUInt64(const PROPVARIANT &property);
        static TString GetString(const PROPVARIANT &property);
        static GUID GetGUID(const PROPVARIANT &property);
        static std::vector<BYTE> GetBytes(const PROPVARIANT &property);
        static UINT64 GetConvertedUInt64(const PROPVARIANT &property);
    };
}

#endif // PROPID_H