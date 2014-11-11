// ArchiveExtractCallback.h

#ifndef SEVENZIPCORE_ARCHIVEEXTRACTCALLBACK_H
#define SEVENZIPCORE_ARCHIVEEXTRACTCALLBACK_H

#include <memory>

#include <boost/optional.hpp>

#include "Archive.h"
#include "COM.h"
#include "FileStream.h"
#include "IArchive.h"
#include "IArchiveAdapter.h"

namespace SevenZipCore
{
    class IExtractIndicator;

    class ArchiveExtractCallback
        : public IArchiveExtractCallback
    {
    public:
        ArchiveExtractCallback(
            std::shared_ptr<Archive> spArchive,
            bool isStandardOutMode,
            bool isTestMode,
            bool isCRCMode,
            TString tstrExtractFolder,
            TString tstrCurrentBeginPath,
            ExtractPathMode pathMode,
            ExtractOverwriteMode overwriteMode,
            IExtractIndicator *extractIndicator);

        STDMETHOD(SetTotal)(UINT64 total);
        STDMETHOD(SetCompleted)(const UINT64 *completeValue);
        STDMETHOD(GetStream)(
            UINT32 index,
            ISequentialOutStream **outStream,
            INT32 askExtractMode);

        STDMETHOD(PrepareOperation)(INT32 askExtractMode);
        STDMETHOD(SetOperationResult)(INT32 operationResult);

        const TString &GetExtractFolder() const { return m_tstrExtractFolder; }
        const TString &GetLastExtractPath() const { return m_tstrExtractPath; }

        IUNKNOWN_IMP1(IArchiveExtractCallback)

    private:
        std::shared_ptr<Archive> m_spArchive;
        bool m_isStandardOutMode;
        bool m_isTestMode;
        bool m_isCRCMode;
        bool m_isExtractMode;
        TString m_tstrExtractFolder;
        std::vector<TString> m_vtstrCurrentBeginPathPart;
        UINT64 m_un64Total;
        UINT32 m_nindex;
        bool m_isEncrypted;
        bool m_isSplit;
        TString m_tstrInternalPath;
        bool m_isDirectory;
        boost::optional<UINT64> m_oun64Size;
        boost::optional<UINT64> m_oun64Position;
        boost::optional<UINT32> m_ounAttribute;
        boost::optional<FILETIME> m_oftCreated;
        boost::optional<FILETIME> m_oftAccessed;
        boost::optional<FILETIME> m_oftModified;
        ExtractPathMode m_pathMode;
        ExtractOverwriteMode m_overwriteMode;
        IExtractIndicator *m_pExtractIndicator;

        TString m_tstrExtractPath;
        std::shared_ptr<OutFileStream> m_cpOutStream;

        static boost::optional<FILETIME> __GetTime(
            IInArchiveAdapter<> &archiveAdapter,
            UINT32 index,
            PropertyId propertyId);

    };
}

#endif // SEVENZIPCORE_ARCHIVEEXTRACTCALLBACK_H
