// ComPtr.h

#ifndef COMPTR_H
#define COMPTR_H

#define BOOST_MEM_FN_ENABLE_STDCALL

#include <functional>
#include <memory>

#include <boost/mem_fn.hpp>

#include <Windows.h>

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
        pCom->Release();
    }

    template<typename T>
    std::shared_ptr<T> MakeComPtr(T *pCom)
    {
        TStringStream tss;
        tss << TEXT("ComAddRef: 0x") << std::hex << pCom << TEXT('\n');
        OutputDebugString(tss.str().c_str());
        pCom->AddRef();
        return std::shared_ptr<T>(pCom, ComRelease<T>);
    }
#else
    template<typename T>
    std::shared_ptr<T> MakeComPtr(T *pCom)
    {
        pCom->AddRef();
        return std::shared_ptr<T>(pCom, boost::mem_fn(&T::Release));
    }
#endif
}

#endif // COMPTR_H
