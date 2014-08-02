// ArchiveExtractCallback.h

#ifndef ARCHIVEEXTRACTCALLBACK_H
#define ARCHIVEEXTRACTCALLBACK_H

#include <memory>

#include <boost/optional.hpp>

#include "ArchiveEntry.h"
#include "COM.h"
#include "IArchive.h"

namespace SevenZipCore
{
    class ArchiveExtractCallback
        : public IArchiveExtractCallback
    {
    public:
        ArchiveExtractCallback(std::shared_ptr<ArchiveEntry> spArchiveEntry
            , bool isStandardOutMode
            , bool isTestMode
            , bool isCRC
            , TString tstrExtractPath
            , TString tstrInternalPath
            , PathMode pathMode
            , OverwriteMode overwriteMode);

        STDMETHOD(SetTotal)(UINT64 total);
        STDMETHOD(SetCompleted)(const UINT64 *completeValue);
        STDMETHOD(GetStream)(UINT32 index, ISequentialOutStream **outStream, INT32 askExtractMode);

        STDMETHOD(PrepareOperation)(INT32 askExtractMode);
        STDMETHOD(SetOperationResult)(INT32 resultEOperationResult);

        IUNKNOWN_IMP

    private:
        std::shared_ptr<ArchiveEntry> m_spArchiveEntry;
        bool m_isStandardOutMode;
        bool m_isTestMode;
        bool m_isCRCMode;
        TString m_tstrExtractPath;
        std::vector<TString> m_vtstrInternalPathPart;
        UINT64 m_un64Total;
        UINT32 m_nindex;
        bool m_isEncrypted;
        bool m_isSplit;
        TString m_tstrInternalPath;
        bool m_isDirectory;
        boost::optional<UINT64> m_oun64Position;
        boost::optional<UINT64> m_oun64Size;
        boost::optional<UINT32> m_ounAttribute;
        boost::optional<FILETIME> m_oftCreated;
        boost::optional<FILETIME> m_oftAccessed;
        boost::optional<FILETIME> m_oftModified;
        PathMode m_pathMode;
        OverwriteMode m_overwriteMode;

        static boost::optional<FILETIME> __GetTime(IInArchiveAdapter &archiveAdapter, UINT32 index, PropertyId propertyId);

    };
}

#endif // ARCHIVEEXTRACTCALLBACK_H
