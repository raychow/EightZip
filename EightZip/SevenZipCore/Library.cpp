#include "stdwx.h"
#include "Library.h"

#include "Exception.h"

namespace SevenZipCore
{
    Library::Library(const TString &tstrPath)
        : Library(_CheckLibrary(::LoadLibrary(tstrPath.c_str())))
    {
    }

    Library::Library(const TString &tstrPath, DWORD dwFlags)
        : Library(_CheckLibrary(::LoadLibraryEx(tstrPath.c_str(), nullptr, dwFlags)))
    {
    }

    Library::Library(HMODULE hModule)
        : m_sphModule(hModule, &::FreeLibrary)
    {
    }

    Library::~Library()
    {
    }

    HMODULE Library::_CheckLibrary(HMODULE hModule)
    {
        if (!hModule)
        {
            throw LibraryException("Cannot load the library.");
        }
        return hModule;
    }

    CodecLibrary::CodecLibrary(const TString &tstrPath)
        : Library(tstrPath)
        , m_createObject(GetProc<CreateObjectFunc>("CreateObject"))
        , m_getMethodProperty(GetProc<GetMethodPropertyFunc>("GetMethodProperty"))
    {
    }

    CodecLibrary::CodecLibrary(const TString &tstrPath, DWORD dwFlags)
        : Library(tstrPath, dwFlags)
        , m_createObject(GetProc<CreateObjectFunc>("CreateObject"))
        , m_getMethodProperty(GetProc<GetMethodPropertyFunc>("GetMethodProperty"))
    {
    }

    PROPVARIANT CodecLibrary::GetMethodProperty(UINT32 index, PROPID propId) const
    {
        PROPVARIANT result = {};
        if (S_OK != m_getMethodProperty(index, propId, &result))
        {
            throw LibraryException("Cannot get method property.");
        }
        return result;
    }

}