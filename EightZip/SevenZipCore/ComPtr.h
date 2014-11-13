// ComPtr.h

#ifndef SEVENZIPCORE_COMPTR_H
#define SEVENZIPCORE_COMPTR_H

#define BOOST_MEM_FN_ENABLE_STDCALL

#include <functional>
#include <memory>

#include "Platform.h"

#ifdef _DEBUG
#include "TString.h"
#endif

namespace SevenZipCore
{
    template<typename T>
    using unique_com_ptr = std::unique_ptr<T, std::function<void(T *)>>;

#ifdef _DEBUG
    template<typename T>
    void ComRelease(T *pCom)
    {
        TStringStream tss;
        tss << TEXT("ComRelease: 0x") << std::hex << pCom << TEXT('\n');
        OutputDebugString(tss.str().c_str());
        if (pCom)
        {
            pCom->Release();
        }
    }

    template<typename T>
    std::shared_ptr<T> MakeSharedCom(T *pCom, bool isAddRef = true)
    {
        TStringStream tss;
        tss << TEXT("ComAddRef: 0x") << std::hex << pCom << TEXT('\n');
        OutputDebugString(tss.str().c_str());
        if (pCom)
        {
            if (isAddRef)
            {
                pCom->AddRef();
            }
            return std::shared_ptr<T>(pCom, ComRelease<T>);
        }
        return nullptr;
    }

    template<typename T>
    unique_com_ptr<T> MakeUniqueCom(T *pCom, bool isAddRef = true)
    {
        TStringStream tss;
        tss << TEXT("ComAddRef: 0x") << std::hex << pCom << TEXT('\n');
        OutputDebugString(tss.str().c_str());
        if (pCom)
        {
            if (isAddRef)
            {
                pCom->AddRef();
            }
            return unique_com_ptr<T>(pCom, ComRelease<T>);
        }
        return nullptr;
    }
#else
    template<typename T>
    void ComRelease(T *pCom)
    {
        if (pCom)
        {
            pCom->Release();
        }
    }

    template<typename T>
    std::shared_ptr<T> MakeSharedCom(T *pCom, bool isAddRef = true)
    {
        if (pCom)
        {
            if (isAddRef)
            {
                pCom->AddRef();
            }
            return std::shared_ptr<T>(pCom, std::mem_fn(&T::Release));
        }
        return nullptr;
    }

    template<typename T>
    unique_com_ptr<T> MakeUniqueCom(T *pCom, bool isAddRef = true)
    {
        if (pCom)
        {
            if (isAddRef)
            {
                pCom->AddRef();
            }
            return unique_com_ptr<T>(pCom, ComRelease<T>);
        }
        return nullptr;
    }
#endif
}

#endif // SEVENZIPCORE_COMPTR_H
