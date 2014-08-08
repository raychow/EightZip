// Codecs.h

#ifndef SEVENZIPCORE_CODECS_H
#define SEVENZIPCORE_CODECS_H

#include <memory>
#include <string>
#include <vector>

#include "Platform.h"

#include "COM.h"
#include "Library.h"
#include "IArchive.h"
#include "ICoder.h"
#include "TString.h"

namespace SevenZipCore
{
    //typedef IInArchive * (*CreateInArchiveFunc)();
    //typedef IOutArchive * (*CreateOutArchiveFunc)();

    typedef HRESULT(WINAPI *GetNumberOfMethodsFunc)(UINT32 *numMethods);
    typedef HRESULT(WINAPI *GetNumberOfFormatsFunc)(UINT32 *numFormats);
    typedef HRESULT(WINAPI *GetHandlerPropertyFunc)(
        PROPID propId, PROPVARIANT *value);
    typedef HRESULT(WINAPI *GetHandlerProperty2Func)(
        UINT32 index, PROPID propId, PROPVARIANT *value);
    typedef HRESULT(WINAPI *CreateObjectFunc)(
        const GUID *clsId, const GUID *iid, void **outObject);
    typedef HRESULT(WINAPI *SetLargePageModeFunc)();

    class LibraryPropertyReader
    {
    public:
        LibraryPropertyReader(
            GetHandlerPropertyFunc getHandlerProperty,
            GetHandlerProperty2Func getHandlerProperty2);

        PROPVARIANT ReadProperty(UINT32 unIndex, PROPID propId) const;
    private:
        GetHandlerPropertyFunc m_getHandlerProperty;
        GetHandlerProperty2Func m_getHandlerProperty2;
    };

    class CodecFormat
    {
    public:
        CodecFormat(
            const CodecLibrary &codecLibrary,
            UINT unFormatIndex,
            const LibraryPropertyReader &propertyReader);
        virtual ~CodecFormat();

        const CodecLibrary &GetCodecLibrary() const { return m_codecLibrary; }
        UINT32 GetFormatIndex() const { return m_unFormatIndex; }
        CLSID GetClassId() const { return m_classId; }
        bool IsUpdateEnbaled() const { return m_isUpdateEnbaled; }
        const TString &GetName() const { return m_tstrName; }
        const std::vector<std::unique_ptr<ExtensionInfo>> &GetExtensions() const
        {
            return m_vupExtensionInfo;
        }
        const std::vector<BYTE> &Signature() const { return m_vbySignature; }
        bool IsKeepName() const { return m_isKeepName; }

        std::shared_ptr<IInArchive> CreateInArchive() const;
        std::shared_ptr<IOutArchive> CreateOutArchive() const;
    private:
        const CodecLibrary &m_codecLibrary;
        UINT32 m_unFormatIndex = 0;
        CLSID m_classId;
        bool m_isUpdateEnbaled = false;
        //CreateInArchiveFunc m_createInArchive;
        //CreateOutArchiveFunc m_createOutArchive;
        TString m_tstrName;
        std::vector<std::unique_ptr<ExtensionInfo>> m_vupExtensionInfo;
        std::vector<BYTE> m_vbySignature;
        bool m_isKeepName = false;

        static void __SplitString(
            const TString &tstrSource,
            std::vector<TString> &vtstrDestination,
            TCHAR wchDelimiter);
        void __AddExtension(
            const TString &tstrExtension,
            const TString &tstrAdditionExtension);
    };

    class CodecInfo
    {
    public:
        CodecInfo(const CodecLibrary &codecLibrary, UINT32 unCodecIndex);
        virtual ~CodecInfo();

        const CodecLibrary &GetCodecLibrary() const { return m_codecLibrary; }
        UINT32 GetCodecIndex() const{ return m_unCodecIndex; }
        bool IsEncoderAssigned() const{ return m_isEncoderAssigned; }
        bool IsDecoderAssigned() const { return m_isDecoderAssigned; }
        const CLSID &GetEncoder() const { return m_clsidEncoder; }
        const CLSID &GetDecoder() const { return m_clsidDecoder; }
    private:
        const CodecLibrary &m_codecLibrary;
        UINT32 m_unCodecIndex;
        bool m_isEncoderAssigned;
        bool m_isDecoderAssigned;
        CLSID m_clsidEncoder;
        CLSID m_clsidDecoder;

        CLSID __GetCodecClass(UINT32 unIndex, PROPID propId);
    };

    class Codecs
        : public ICompressCodecsInfo
    {
    public:
        static const TString MAIN_DLL;
        static const TString CODECS_FOLDER;
        static const TString FORMATS_FOLDER;

        Codecs(const TString &tstrBaseFolder);
        virtual ~Codecs();

        const std::vector<std::unique_ptr<CodecLibrary>> &GetLibraries() const
        {
            return m_vupCodecLibrary;
        }
        const std::vector<std::unique_ptr<CodecInfo>> &GetInfo() const
        {
            return m_vupCodecInfo;
        }
        const std::vector<std::unique_ptr<CodecFormat>> &GetFormats() const
        {
            return m_vupCodecFormat;
        }

        STDMETHOD(GetNumberOfMethods)(UINT32 *unNumberOfMethods);
        STDMETHOD(GetProperty)(
            UINT32 unIndex, PROPID propId, PROPVARIANT *value);
        STDMETHOD(CreateEncoder)(
            UINT32 unIndex, const GUID *interfaceId, void **coder);
        STDMETHOD(CreateDecoder)(
            UINT32 unIndex, const GUID *interfaceId, void **coder);

        //IUNKNOWN_IMP

        QUERYINTERFACE_BEGIN
            QUERYINTERFACE_ENTRY_UNKNOWN(IUnknown)
        QUERYINTERFACE_END
        STDMETHOD_(ULONG, AddRef)()
        {
            return ++m_ulRefCount;
        }
        STDMETHOD_(ULONG, Release)()
        {
            if (--m_ulRefCount)
            {
                return m_ulRefCount;
            }
            delete this;
            return 0;
        }

    private:
        ULONG m_ulRefCount = 0;

    private:
        std::vector<std::unique_ptr<CodecLibrary>> m_vupCodecLibrary;
        std::vector<std::unique_ptr<CodecInfo>> m_vupCodecInfo;
        std::vector<std::unique_ptr<CodecFormat>> m_vupCodecFormat;

        void __LoadDLL(const TString &tstrPath);
        // TODO: Implement
        //void __LoadDLLsFormFolder(const tstring &tstrPath);
        UINT32 __LoadCodecs(const CodecLibrary &codecLibrary);
        UINT32 __LoadFormats(const CodecLibrary &codecLibrary);
    };
}

#endif // SEVENZIPCORE_CODECS_H
