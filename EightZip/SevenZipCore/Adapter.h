// Adapter.h

#ifndef SEVENZIPCORE_ADAPTER_H
#define SEVENZIPCORE_ADAPTER_H

#include <Windows.h>

namespace SevenZipCore
{
    template < typename T >
    class Adapter
    {
    public:
        explicit Adapter(std::shared_ptr<T> spTarget)
            : m_spTarget(spTarget)
        {

        }

        template<typename I>
        std::shared_ptr<I> QueryInterface(REFGUID iid)
        {
            I *pResult = nullptr;
            if (SUCCEEDED(m_spTarget->QueryInterface(
                iid, reinterpret_cast<void **>(&pResult))))
            {
                return MakeComPtr(pResult, false);
            }
            return nullptr;
        }

        inline std::shared_ptr<T> GetTarget() const
        {
            return m_spTarget;
        }

        virtual ~Adapter() { };

    private:
        std::shared_ptr<T> m_spTarget;

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
