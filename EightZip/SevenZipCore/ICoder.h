// ICoder.h

#ifndef SEVENZIPCORE_ICODER_H
#define SEVENZIPCORE_ICODER_H

#include "IStream.h"

namespace SevenZipCore
{
#define CODER_INTERFACE(i, x) DECL_INTERFACE(i, 4, x)

    CODER_INTERFACE(ICompressProgressInfo, 0x04)
    {
        STDMETHOD(SetRatioInfo)(
            const UINT64 *inSize, const UINT64 *outSize) PURE;
    };

    CODER_INTERFACE(ICompressCoder, 0x05)
    {
        STDMETHOD(Code)(ISequentialInStream *inStream,
            ISequentialOutStream *outStream,
            const UINT64 *inSize,
            const UINT64 *outSize,
            ICompressProgressInfo *progress) PURE;
    };

    CODER_INTERFACE(ICompressCoder2, 0x18)
    {
        STDMETHOD(Code)(ISequentialInStream **inStreams,
            const UINT64 **inSizes,
            UINT32 numInStreams,
            ISequentialOutStream **outStreams,
            const UINT64 **outSizes,
            UINT32 numOutStreams,
            ICompressProgressInfo *progress) PURE;
    };

    namespace NCoderPropID
    {
        enum EEnum
        {
            kDefaultProp = 0,
            kDictionarySize,
            kUsedMemorySize,
            kOrder,
            kBlockSize,
            kPosStateBits,
            kLitContextBits,
            kLitPosBits,
            kNumFastBytes,
            kMatchFinder,
            kMatchFinderCycles,
            kNumPasses,
            kAlgorithm,
            kNumThreads,
            kEndMarker
        };
    }

    CODER_INTERFACE(ICompressSetCoderProperties, 0x20)
    {
        STDMETHOD(SetCoderProperties)(const PROPID *propIDs,
            const PROPVARIANT *props,
            UINT32 numProps) PURE;
    };

    /*
    CODER_INTERFACE(ICompressSetCoderProperties, 0x21)
    {
    STDMETHOD(SetDecoderProperties)(ISequentialInStream *inStream) PURE;
    };
    */

    CODER_INTERFACE(ICompressSetDecoderProperties2, 0x22)
    {
        STDMETHOD(SetDecoderProperties2)(
            const BYTE *data, UINT32 size) PURE;
    };

    CODER_INTERFACE(ICompressWriteCoderProperties, 0x23)
    {
        STDMETHOD(WriteCoderProperties)(ISequentialOutStream *outStream) PURE;
    };

    CODER_INTERFACE(ICompressGetInStreamProcessedSize, 0x24)
    {
        STDMETHOD(GetInStreamProcessedSize)(UINT64 *value) PURE;
    };

    CODER_INTERFACE(ICompressSetCoderMt, 0x25)
    {
        STDMETHOD(SetNumberOfThreads)(UINT32 numThreads) PURE;
    };

    CODER_INTERFACE(ICompressGetSubStreamSize, 0x30)
    {
        STDMETHOD(GetSubStreamSize)(UINT64 subStream, UINT64 *value) PURE;
    };

    CODER_INTERFACE(ICompressSetInStream, 0x31)
    {
        STDMETHOD(SetInStream)(ISequentialInStream *inStream) PURE;
        STDMETHOD(ReleaseInStream)() PURE;
    };

    CODER_INTERFACE(ICompressSetOutStream, 0x32)
    {
        STDMETHOD(SetOutStream)(ISequentialOutStream *outStream) PURE;
        STDMETHOD(ReleaseOutStream)() PURE;
    };

    CODER_INTERFACE(ICompressSetInStreamSize, 0x33)
    {
        STDMETHOD(SetInStreamSize)(const UINT64 *inSize) PURE;
    };

    CODER_INTERFACE(ICompressSetOutStreamSize, 0x34)
    {
        STDMETHOD(SetOutStreamSize)(const UINT64 *outSize) PURE;
    };

    CODER_INTERFACE(ICompressSetBufSize, 0x35)
    {
        STDMETHOD(SetInBufSize)(UINT32 streamIndex, UINT32 size) PURE;
        STDMETHOD(SetOutBufSize)(UINT32 streamIndex, UINT32 size) PURE;
    };

    CODER_INTERFACE(ICompressFilter, 0x40)
    {
        STDMETHOD(Init)() PURE;
        STDMETHOD_(UINT32, Filter)(BYTE *data, UINT32 size) PURE;
        // Filter converts as most as possible bytes
        // Filter return outSize (UINT32)
        // if (outSize <= size): Filter have converted outSize bytes
        // if (outSize > size): Filter have not converted anything.
        //      and it needs at least outSize bytes to convert one block
        //      (it's for crypto block algorithms).
    };

    CODER_INTERFACE(ICompressCodecsInfo, 0x60)
    {
        STDMETHOD(GetNumberOfMethods)(UINT32 *numMethods) PURE;
        STDMETHOD(GetProperty)(
            UINT32 index, PROPID propID, PROPVARIANT *value) PURE;
        STDMETHOD(CreateDecoder)(
            UINT32 index, const GUID *iid, void **coder) PURE;
        STDMETHOD(CreateEncoder)(
            UINT32 index, const GUID *iid, void **coder) PURE;
    };
    CODER_INTERFACE(ISetCompressCodecsInfo, 0x61)
    {
        STDMETHOD(SetCompressCodecsInfo)(
            ICompressCodecsInfo *compressCodecsInfo) PURE;
    };

    CODER_INTERFACE(ICryptoProperties, 0x80)
    {
        STDMETHOD(SetKey)(const BYTE *data, UINT32 size) PURE;
        STDMETHOD(SetInitVector)(const BYTE *data, UINT32 size) PURE;
    };

    /*
    CODER_INTERFACE(ICryptoResetSalt, 0x88)
    {
    STDMETHOD(ResetSalt)() PURE;
    };
    */

    CODER_INTERFACE(ICryptoResetInitVector, 0x8C)
    {
        STDMETHOD(ResetInitVector)() PURE;
    };

    CODER_INTERFACE(ICryptoSetPassword, 0x90)
    {
        STDMETHOD(CryptoSetPassword)(const BYTE *data, UINT32 size) PURE;
    };

    CODER_INTERFACE(ICryptoSetCRC, 0xA0)
    {
        STDMETHOD(CryptoSetCRC)(UINT32 crc) PURE;
    };

    //////////////////////
    // It's for DLL file
    enum struct MethodPropertyId
    {
        ID,
        Name,
        Decoder,
        Encoder,
        InStreams,
        OutStreams,
        Description,
        IsDecoderAssigned,
        IsEncoderAssigned
    };
}

#endif // SEVENZIPCORE_ICODER_H
