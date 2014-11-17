// Adapter.h

#ifndef SEVENZIPCORE_ADAPTER_H
#define SEVENZIPCORE_ADAPTER_H

#include <Windows.h>

#include "ComPtr.h"

namespace SevenZipCore
{
    template<typename T>
    class Adapter
    {
    public:
        explicit Adapter(T &target)
            : m_target(target)
        {

        }
        virtual ~Adapter() {}

        template<typename I>
        unique_com_ptr<I> QueryInterface(REFGUID iid)
        {
            I *pResult = nullptr;
            if (SUCCEEDED(m_target.QueryInterface(
                iid, reinterpret_cast<void **>(&pResult))))
            {
                return MakeUniqueCom(pResult, false);
            }
            return nullptr;
        }

        inline T &GetTarget() const
        {
            return m_target;
        }


    private:
        T &m_target;

    };

    template<typename E>
    inline void EnsureOk(HRESULT result, const char *message)
    {
        if (S_OK != result)
        {
            throw E(message);
        }
    }
    
}

#endif // SEVENZIPCORE_ADAPTER_H
