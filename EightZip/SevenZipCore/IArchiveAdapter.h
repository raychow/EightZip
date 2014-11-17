// IArchiveAdapter.h

#ifndef SEVENZIPCORE_IARCHIVEADAPTER_H
#define SEVENZIPCORE_IARCHIVEADAPTER_H

#include <memory>
#include <vector>

#include "Adapter.h"
#include "Exception.h"
#include "IArchive.h"
#include "Property.h"
#include "TString.h"

namespace SevenZipCore
{
    template<>
    inline void EnsureOk<ArchiveException>(HRESULT result, const char *message)
    {
        if (S_OK != result)
        {
            throw ArchiveException(message, result);
        }
    }

    template<typename T = IInArchive>
    class IInArchiveAdapter
        : protected virtual Adapter<T>
    {
    public:
        explicit IInArchiveAdapter(T &target)
            : Adapter(target)
        {
        }

        using Adapter<T>::QueryInterface;
        using Adapter<T>::GetTarget;
            
        void Open(
            IInStream *stream,
            UINT64 maxCheckStartPosition,
            IArchiveOpenCallback *openArchiveCallback) const
        {
            EnsureOk<ArchiveException>(GetTarget().Open(
                stream, &maxCheckStartPosition, openArchiveCallback),
                "Cannot open the specified stream.");
        }

        void Close() const
        {
            EnsureOk<ArchiveException>(GetTarget().Close(),
                "Cannot close the archive.");
        }

        UINT32 GetNumberOfItems() const
        {
            UINT32 result = 0;
            EnsureOk<ArchiveException>(GetTarget().GetNumberOfItems(&result),
                "Cannot get number of archive items");
            return result;
        }

        PROPVARIANT GetProperty(UINT32 index, PropertyId propertyID) const
        {
            PROPVARIANT result = {};
            EnsureOk<ArchiveException>(GetTarget().GetProperty(
                index,
                static_cast<PROPID>(propertyID), &result),
                "Cannot get property of the file in archive.");
            return result;
        }

        void Extract(
            const std::vector<UINT32> &indices,
            bool testMode,
            IArchiveExtractCallback *extractCallback) const
        {
            EnsureOk<ArchiveException>(GetTarget().Extract(
                indices.data(),
                indices.size(),
                testMode ? 1 : 0,
                extractCallback),
                "Can not extract files from the archive.");
        }

        void ExtractAll(
            bool testMode,
            IArchiveExtractCallback *extractCallback) const
        {
            EnsureOk<ArchiveException>(GetTarget().Extract(
                nullptr,
                -1,
                testMode ? 1 : 0,
                extractCallback),
                "Can not extract files from the archive.");
        }

        PROPVARIANT GetArchiveProperty(PropertyId propID) const
        {
            PROPVARIANT result = {};
            EnsureOk<ArchiveException>(GetTarget().GetArchiveProperty(
                static_cast<PROPID>(propertyId),
                &result),
                "Cannot get property of the archive.");
            return result;
        }

        UINT32 GetNumberOfProperties() const
        {
            UINT32 result = 0;
            EnsureOk<ArchiveException>(GetTarget().GetNumberOfProperties(&result),
                "Cannot get number of archive properties");
            return result;
        }

        void GetPropertyInfo(
            UINT32 index,
            BSTR *name,
            PROPID *propID,
            VARTYPE *varType) const
        {
            EnsureOk<ArchiveException>(GetTarget().GetPropertyInfo(
                index, name, propID, varType),
                "Cannot get property info of the archive.");
        }

        UINT32 GetNumberOfArchiveProperties() const
        {
            UINT32 result = 0;
            EnsureOk<ArchiveException>(
                GetTarget().GetNumberOfArchiveProperties(&result),
                "Cannot get number of archive properties.");
            return result;
        }

        void GetArchivePropertyInfo(
            UINT32 index,
            BSTR *name,
            PROPID *propID,
            VARTYPE *varType) const
        {
            EnsureOk<ArchiveException>(GetTarget().GetArchivePropertyInfo(
                index, name, propID, varType),
                "Cannot get property info of the file in archive.");
        }

        TString GetItemPath(UINT32 index) const
        {
            auto tstrPath = PropertyHelper::GetString(
                GetProperty(index, PropertyId::Path), TString());
            if (tstrPath.empty())
            {
                auto tstrExtension = PropertyHelper::GetString(
                    GetProperty(index, PropertyId::Extension), TString());
                if (!tstrExtension.empty())
                {
                    tstrPath = TString(TEXT(".")) + tstrExtension;
                }
            }
            return tstrPath;
        }

    };

    template<typename T = IInArchiveGetStream>
    class IInArchiveGetStreamAdapter
        : protected virtual Adapter<T>
    {
    public:
        explicit IInArchiveGetStreamAdapter(T &target)
            : Adapter(target)
        {
        }

        using Adapter<T>::QueryInterface;
        using Adapter<T>::GetTarget;

        unique_com_ptr<ISequentialInStream> GetStream(UINT32 index) const
        {
            ISequentialInStream *pStream = nullptr;
            EnsureOk<ArchiveException>(
                GetTarget().GetStream(index, &pStream),
                "Cannot get stream.");
            return MakeUniqueCom(pStream, false);
        }

    };

}

#endif // SEVENZIPCORE_IARCHIVEADAPTER_H
