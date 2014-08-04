#include "Codecs.h"

#include <sstream>

#include "ComPtr.h"
#include "Exception.h"
#include "ICoder.h"
#include "Property.h"
#include "PropertyVariant.h"
#include "TString.h"

using namespace std;

namespace SevenZipCore
{
    LibraryPropertyReader::LibraryPropertyReader(
        GetHandlerPropertyFunc getHandlerProperty,
        GetHandlerProperty2Func getHandlerProperty2)
        : m_getHandlerProperty(getHandlerProperty)
        , m_getHandlerProperty2(getHandlerProperty2)
    {
        if (!getHandlerProperty && !getHandlerProperty2)
        {
            throw LibraryException("Assigned two empty functions.");
        }
    }

    PROPVARIANT LibraryPropertyReader::ReadProperty(
        UINT32 unIndex, PROPID propId) const
    {
        PROPVARIANT result = {};
        HRESULT hResult = NULL;
        if (!m_getHandlerProperty)
        {
            hResult = m_getHandlerProperty2(unIndex, propId, &result);
        }
        else
        {
            hResult = m_getHandlerProperty(propId, &result);
        }
        if (hResult != S_OK)
        {
            throw LibraryException("Cannot read property.");
        }
        return result;
    }

    CodecFormat::CodecFormat(const CodecLibrary &codecLibrary,
        UINT unFormatIndex,
        const LibraryPropertyReader &propertyReader)
        : m_codecLibrary(codecLibrary)
        , m_unFormatIndex(unFormatIndex)
        , m_classId()
    {
        TString tstrExtension;
        TString tstrAdditionExtension;
        // Will call std::[w]string's or std::vector's move constructor.
        m_tstrName = PropertyHelper::GetString(propertyReader.ReadProperty(
            m_unFormatIndex, static_cast<PROPID>(FormatInfo::Name)), TString());
        m_classId = PropertyHelper::GetGUID(propertyReader.ReadProperty(
            unFormatIndex, static_cast<PROPID>(FormatInfo::ClassID)));
        __AddExtension(PropertyHelper::GetString(propertyReader.ReadProperty(
            unFormatIndex, static_cast<PROPID>(FormatInfo::Extension)),
            TString()),
            PropertyHelper::GetString(propertyReader.ReadProperty(
            unFormatIndex, static_cast<PROPID>(FormatInfo::AdditionExtension)),
            TString()));

        if (m_isUpdateEnbaled = PropertyHelper::GetBool(
            propertyReader.ReadProperty(
            unFormatIndex, static_cast<PROPID>(FormatInfo::Update)), false))
        {
            m_isKeepName = PropertyHelper::GetBool(propertyReader.ReadProperty(
                unFormatIndex, static_cast<PROPID>(
                FormatInfo::KeepName)), false);

        }
        m_vbySignature = PropertyHelper::GetBytes(propertyReader.ReadProperty(
            unFormatIndex, static_cast<PROPID>(FormatInfo::StartSignature)),
            vector<BYTE>());
    }

    CodecFormat::~CodecFormat()
    {
    }

    std::shared_ptr<IInArchive> CodecFormat::CreateInArchive() const
    {
        return MakeComPtr(m_codecLibrary.CreateObject<IInArchive>(
            m_classId, IID_IInArchive));
    }

    std::shared_ptr<IOutArchive> CodecFormat::CreateOutArchive() const
    {
        return MakeComPtr(m_codecLibrary.CreateObject<IOutArchive>(
            m_classId, IID_IOutArchive));
    }

    void CodecFormat::__SplitString(const TString &tstrSource,
        std::vector<TString> &vtstrDestination, TCHAR wchDelimiter)
    {
        TStringStream tstringStream(tstrSource);
        TString tstrItem;
        while (getline(tstringStream, tstrItem, wchDelimiter))
        {
            vtstrDestination.push_back(tstrItem);
        }
    }

    void CodecFormat::__AddExtension(const TString &tstrExtension,
        const TString &tstrAdditionExtension)
    {
        vector<TString> vtstrExtension;
        vector<TString> vtstrAdditionExtension;
        __SplitString(tstrExtension, vtstrExtension, TEXT(' '));
        __SplitString(tstrAdditionExtension, vtstrAdditionExtension, TEXT(' '));
        auto iterAdditionExtension = vtstrAdditionExtension.cbegin();
        for (const auto &tstrExtension : vtstrExtension)
        {
            auto upExtensionInfo = unique_ptr<ExtensionInfo>(
                new ExtensionInfo(tstrExtension.c_str()));
            if (iterAdditionExtension != vtstrAdditionExtension.cend())
            {
                upExtensionInfo->AdditionExtension = *iterAdditionExtension++;
            }
            m_vupExtensionInfo.push_back(move(upExtensionInfo));
        }
    }

    //const CodecInfo &CodecLibrary::GetCodecInfo(int nIndex) const
    //{
    //    return m_vCodecInfo[nIndex];
    //}

    //int CodecLibrary::GetCodecCount() const
    //{
    //    return m_vCodecInfo.size();
    //}

    CodecInfo::CodecInfo(const CodecLibrary &codecLibrary, UINT32 unCodecIndex)
        : m_codecLibrary(codecLibrary)
        , m_unCodecIndex(unCodecIndex)
        , m_isEncoderAssigned(false)
        , m_isDecoderAssigned(false)
        , m_clsidEncoder(__GetCodecClass(unCodecIndex,
        static_cast<PROPID>(MethodPropertyId::Encoder)))
        , m_clsidDecoder(__GetCodecClass(unCodecIndex,
        static_cast<PROPID>(MethodPropertyId::Decoder)))
    {}

    CodecInfo::~CodecInfo()
    {

    }

    CLSID CodecInfo::__GetCodecClass(UINT32 unIndex, PROPID propId)
    {
        PropertyVariant propVariant = m_codecLibrary.GetMethodProperty(
            unIndex, propId);
        switch (propVariant.vt)
        {
        case VT_BSTR:
            return *reinterpret_cast<const GUID *>(propVariant.bstrVal);
        case VT_EMPTY:
            return CLSID();
        default:
            throw LibraryException("Cannot get the codec class.");
        }
    }

    const TString Codecs::MAIN_DLL = TEXT("7z.dll");
    const TString Codecs::CODECS_FOLDER = TEXT("Codecs");
    const TString Codecs::FORMATS_FOLDER = TEXT("Formats");

    Codecs::Codecs(const TString &tstrBaseFolder)
        : m_vupCodecLibrary()
        , m_vupCodecInfo()
        , m_vupCodecFormat()
    {
        __LoadDLL((tstrBaseFolder + TEXT("/") + MAIN_DLL).c_str());
    }

    Codecs::~Codecs()
    {
    }

    STDMETHODIMP Codecs::GetNumberOfMethods(UINT32 *unNumberOfMethods)
    {
        *unNumberOfMethods = m_vupCodecInfo.size();
        return S_OK;
    }

    STDMETHODIMP Codecs::GetProperty(
        UINT32 unIndex, PROPID propId, PROPVARIANT *value)
    {
        const auto &codecInfo = *m_vupCodecInfo[unIndex];

        if (propId == static_cast<PROPID>(MethodPropertyId::IsDecoderAssigned))
        {
            PropertyVariant propVariant;
            propVariant = codecInfo.IsDecoderAssigned();
            propVariant.Detach(value);
            return S_OK;
        }
        if (propId == static_cast<PROPID>(MethodPropertyId::IsEncoderAssigned))
        {
            PropertyVariant propVariant;
            propVariant = codecInfo.IsEncoderAssigned();
            propVariant.Detach(value);
            return S_OK;
        }
        try
        {
            *value = codecInfo.GetCodecLibrary().GetMethodProperty(
                codecInfo.GetCodecIndex(), propId);
        }
        catch (const SevenZipCoreException &)
        {
            return S_FALSE;
        }
        return S_OK;
    }

    STDMETHODIMP Codecs::CreateEncoder(
        UINT32 unIndex, const GUID *interfaceId, void **coder)
    {
        const auto &codecInfo = *m_vupCodecInfo[unIndex];
        if (codecInfo.IsEncoderAssigned())
        {
            try
            {
                *coder = codecInfo.GetCodecLibrary().CreateObject<void>(
                    codecInfo.GetEncoder(), *interfaceId);
            }
            catch (const SevenZipCoreException &)
            {
                return S_FALSE;
            }
        }
        return S_OK;
    }

    STDMETHODIMP Codecs::CreateDecoder(
        UINT32 unIndex, const GUID *interfaceId, void **coder)
    {
        const auto &codecInfo = *m_vupCodecInfo[unIndex];
        if (codecInfo.IsDecoderAssigned())
        {
            try
            {
                *coder = codecInfo.GetCodecLibrary().CreateObject<void>(
                    codecInfo.GetDecoder(), *interfaceId);
            }
            catch (const SevenZipCoreException &)
            {
                return S_FALSE;
            }
        }
        return S_OK;
    }

    void Codecs::__LoadDLL(const TString &tstrPath)
    {
        auto upCodecLibrary = unique_ptr<CodecLibrary>(
            new CodecLibrary(tstrPath));
        if (__LoadCodecs(*upCodecLibrary) + __LoadFormats(*upCodecLibrary))
        {
            m_vupCodecLibrary.push_back(move(upCodecLibrary));
        }
    }

    //void Codecs::__LoadDLLsFormFolder(const tstring &tstrPath)
    //{
    //    
    //}

    UINT32 Codecs::__LoadCodecs(const CodecLibrary &codecLibrary)
    {
        auto getNumberOfMethods = codecLibrary.GetProc<GetNumberOfMethodsFunc>(
            "GetNumberOfMethods");
        UINT32 unNumberOfMethods;
        if (getNumberOfMethods(&unNumberOfMethods) != S_OK)
        {
            throw LibraryException("Cannot get number of methods.");
        }
        UINT32 result = 0;
        for (UINT32 i = 0; i != unNumberOfMethods; ++i)
        {
            try
            {
                m_vupCodecInfo.push_back(unique_ptr<CodecInfo>(
                    new CodecInfo(codecLibrary, i)));
                ++result;
            }
            catch (const LibraryException &)
            {}
        }
        return result;
    }

    UINT32 Codecs::__LoadFormats(const CodecLibrary &codecLibrary)
    {
        auto getHandlerProperty2
            = codecLibrary.GetProc<GetHandlerProperty2Func>(
            "GetHandlerProperty2", true);
        auto getHandlerProperty = getHandlerProperty2 ?
            nullptr :
            codecLibrary.GetProc<GetHandlerPropertyFunc>("GetHandlerProperty");
        UINT32 unNumberOfFormats = 1u;
        if (!getHandlerProperty)
        {
            auto getNumberOfFormats
                = codecLibrary.GetProc<GetNumberOfFormatsFunc>(
                "GetNumberOfFormats");
            getNumberOfFormats(&unNumberOfFormats);
        }

        UINT32 result = 0;
        LibraryPropertyReader propertyReader(
            getHandlerProperty, getHandlerProperty2);
        for (UINT32 i = 0; i != unNumberOfFormats; ++i)
        {
            try
            {
                m_vupCodecFormat.push_back(unique_ptr<CodecFormat>(
                    new CodecFormat(codecLibrary, i, propertyReader)));
                ++result;
            }
            catch (const LibraryException &)
            {
            }
        }
        return result;
    }
}
