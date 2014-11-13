// IStreamAdapter.h

#ifndef SEVENZIPCORE_ISTREAMADAPTER_H
#define SEVENZIPCORE_ISTREAMADAPTER_H

#include <memory>
#include <vector>

#include "Adapter.h"
#include "IStream.h"

namespace SevenZipCore
{
    template<typename T = IInStream>
    class IInStreamAdapter
        : protected virtual Adapter<T>
    {
    public:
        explicit IInStreamAdapter(T &target)
            : Adapter(target)
        {
        }

        using Adapter<T>::QueryInterface;
        using Adapter<T>::GetTarget;

        std::vector<char> Read(UINT32 size) const;
        UINT64 Seek(INT64 offset, UINT32 seekOrigin) const;

    };

    template<typename T = IOutStream>
    class IOutStreamAdapter
        : protected virtual Adapter<T>
    {
    public:
        explicit IOutStreamAdapter(T &target)
            : Adapter(target)
        {
        }

        using Adapter<T>::QueryInterface;
        using Adapter<T>::GetTarget;

        UINT32 Write(std::vector<char> data) const;
        UINT64 Seek(INT64 offset, UINT32 seekOrigin) const;
        void SetSize(UINT64 newSize) const;

    };

    template<typename T = IStreamGetSize>
    class IStreamGetSizeAdapter
        : protected virtual Adapter<T>
    {
    public:
        explicit IStreamGetSizeAdapter(T &target)
            : Adapter(target)
        {
        }

        using Adapter<T>::QueryInterface;
        using Adapter<T>::GetTarget;

        UINT64 GetSize() const;

    };

    template<typename T>
    std::vector<char> IInStreamAdapter<T>::Read(UINT32 size) const
    {
        std::vector<char> result(size);
        UINT32 unReadSize = 0;
        EnsureOk<StreamException>(
            GetTarget().Read(result.data(), size, &unReadSize),
            "Cannot read the file stream.");
        result.resize(size);
        return result;
    }

    template<typename T>
    UINT64 IInStreamAdapter<T>::Seek(INT64 offset, UINT32 seekOrigin) const
    {
        UINT64 result = 0;
        EnsureOk<StreamException>(
            static_cast<bool>(GetTarget().Seek(offset, seekOrigin, &result)),
            "Cannot seek the file stream.");
        return result;
    }

    template<typename T>
    UINT32 IOutStreamAdapter<T>::Write(std::vector<char> data) const
    {
        UINT32 result = 0;
        EnsureOk<StreamException>(
            GetTarget().Write(data.data(), data.size(), &result),
            "Cannot write to the file stream.");
        return result;
    }

    template<typename T>
    UINT64 IOutStreamAdapter<T>::Seek(INT64 offset, UINT32 seekOrigin) const
    {
        UINT64 result = 0;
        EnsureOk<StreamException>(
            GetTarget().Seek(offset, seekOrigin, &result),
            "Cannot seek the file stream.");
        return result;
    }

    template<typename T>
    void IOutStreamAdapter<T>::SetSize(UINT64 newSize) const
    {
        EnsureOk<StreamException>(GetTarget().SetSize(newSize),
            "Cannot set size of the file stream.");
    }

    template<typename T>
    UINT64 IStreamGetSizeAdapter<T>::GetSize() const
    {
        UINT64 result = 0;
        EnsureOk<StreamException>(GetTarget().GetSize(&result),
            "Cannot get size of the file stream.");
        return result;
    }

}

#endif // SEVENZIPCORE_ISTREAMADAPTER_H
