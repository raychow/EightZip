// Library.h

#ifndef SEVENZIPCORE_LIBRARY_H
#define SEVENZIPCORE_LIBRARY_H

#include <functional>
#include <memory>

#include "Platform.h"

#include "Exception.h"
#include "TString.h"

namespace SevenZipCore
{
    typedef HRESULT(WINAPI *GetMethodPropertyFunc)(
        UINT32 index, PROPID propId, PROPVARIANT *value);
    typedef HRESULT(WINAPI *CreateObjectFunc)(
        const GUID *classId, const GUID *interfaceId, void **outObject);

    class Library
    {
    public:
        Library(const TString &tstrPath);
        Library(const TString &tstrPath, DWORD dwFlags);
        virtual ~Library();

        template<typename T>
        T GetProc(
            const std::string &strProcName,
            bool isSuppresseException = false) const;

    protected:
        std::unique_ptr<HINSTANCE__,
            std::function<void(HMODULE) >> m_uphModule;

        HMODULE _CheckLibrary(HMODULE hModule);

    };

    class CodecLibrary
        : public Library
    {
    public:
        CodecLibrary(const TString &tstrPath);
        CodecLibrary(const TString &tstrPath, DWORD dwFlags);

        template<typename T>
        T* CreateObject(const GUID &classId, const GUID &interfaceId) const;

        PROPVARIANT GetMethodProperty(UINT32 index, PROPID propId) const;
    private:
        const CreateObjectFunc m_createObject;
        const GetMethodPropertyFunc m_getMethodProperty;
    };

    template<typename T>
    T Library::GetProc(const std::string &strProcName,
        bool isSuppresseException /*= false*/) const
    {
        auto result = ::GetProcAddress(m_uphModule.get(), strProcName.c_str());
        if (!isSuppresseException && !result)
        {
            throw LibraryException(
                (std::string("Cannot get function \"") 
                + strProcName + "\".").c_str());
        }
        return reinterpret_cast<T>(result);
    }

    template<typename T>
    T *CodecLibrary::CreateObject(
        const GUID &classId, const GUID &interfaceId) const
    {
        void *pOutObject = nullptr;
        if (S_OK != m_createObject(&classId, &interfaceId, &pOutObject) || !pOutObject)
        {
            throw LibraryException("The library cannot create object.");
        }
        return reinterpret_cast<T *>(pOutObject);
    }

    class ExtensionInfo
    {
    public:
        ExtensionInfo(TString tstrExtension)
            : Extension(tstrExtension)
        {}

        ExtensionInfo(TString tstrExtension, TString tstrAdditionExtension)
            : Extension(tstrExtension)
            , AdditionExtension(tstrAdditionExtension)
        {}

        TString Extension;
        TString AdditionExtension;
    };
}

#endif // SEVENZIPCORE_LIBRARY_H
