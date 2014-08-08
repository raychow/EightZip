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
    std::shared_ptr<T> MakeComPtr(T *pCom)
    {
        TStringStream tss;
        tss << TEXT("ComAddRef: 0x") << std::hex << pCom << TEXT('\n');
        OutputDebugString(tss.str().c_str());
        if (pCom)
        {
            pCom->AddRef();
        }
        return std::shared_ptr<T>(pCom, ComRelease<T>);
    }
#else
    template<typename T>
    std::shared_ptr<T> MakeComPtr(T *pCom)
    {
        if (pCom)
        {
            pCom->AddRef();
            return std::shared_ptr<T>(pCom, std::mem_fn(&T::Release));
        }
        else
        {
            return nullptr;
        }
    }
#endif
}

#endif // SEVENZIPCORE_COMPTR_H
