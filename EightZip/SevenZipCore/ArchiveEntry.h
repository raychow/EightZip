#ifndef SEVENZIPCORE_ARCHIVEENTRY_H
#define SEVENZIPCORE_ARCHIVEENTRY_H

#include <memory>

#include <boost/optional.hpp>

#include "Codecs.h"
#include "ComPtr.h"
#include "IArchiveAdapter.h"
#include "IStream.h"
#include "TString.h"

namespace SevenZipCore
{
    class ArchiveEntry
    {
    public:
        ArchiveEntry(
            Codecs &codecs,
            TString tstrPath,
            std::shared_ptr<IArchiveOpenCallback> cpCallback);
        ArchiveEntry(Codecs &codecs,
            TString tstrPath,
            std::shared_ptr<IInStream> cpStream,
            int nSubFileIndex,
            std::shared_ptr<IArchiveOpenCallback> cpCallback);
        virtual ~ArchiveEntry();

        int GetSubfileIndex() const { return m_nSubfileIndex; }
        void SetSubfileIndex(int value) { m_nSubfileIndex = value; }

        std::shared_ptr<IInArchive> GetArchive() const { return m_cpArchive; }
    private:
        static const UINT64 MAX_CHECK_START_POSITION = 1 << 22;

        Codecs &m_codecs;

        TString m_tstrPath;
        std::shared_ptr<IInStream> m_cpInStream;
        int m_nSubfileIndex = -1;

        std::shared_ptr<IInArchive> m_cpArchive;
        std::shared_ptr<IArchiveOpenCallback> m_cpCallback;

        boost::optional<FILETIME> m_oftModified;

        void __OpenFile();
        void __OpenStream();
        void __Close();
    };
}

#endif
