#ifndef SEVENZIPCORE_ARCHIVEENTRY_H
#define SEVENZIPCORE_ARCHIVEENTRY_H

#include <memory>

#include <boost/optional.hpp>

#include "Platform.h"
#include "TString.h"

namespace SevenZipCore
{
    class Archive;
    class ArchiveFolder;
    class Codecs;
    struct IArchiveOpenCallback;
    struct IInArchive;
    struct IInStream;

    // CArc
    class ArchiveEntry
        : public std::enable_shared_from_this<ArchiveEntry>
    {
    public:
        ArchiveEntry(
            std::weak_ptr<Archive> wpArchive,
            std::shared_ptr<Codecs> cpCodecs,
            TString tstrPath,
            std::shared_ptr<IInStream> cpStream,
            int nSubFileIndex,
            std::shared_ptr<IArchiveOpenCallback> cpCallback);
        virtual ~ArchiveEntry();

        std::shared_ptr<Archive> GetArchive() const
        {
            return m_wpArchive.lock();
        }

        int GetSubfileIndex() const { return m_nSubfileIndex; }
        void SetSubfileIndex(int value) { m_nSubfileIndex = value; }

        std::shared_ptr<IInArchive> GetInArchive() const
        {
            return m_cpInArchive;
        }

        // Do not use it directly since it loads files during every call.
        std::shared_ptr<ArchiveFolder> GetRootFolder();

    private:
        static const UINT64 MAX_CHECK_START_POSITION = 1 << 22;

        std::shared_ptr<Codecs> m_cpCodecs;

        TString m_tstrPath;
        std::shared_ptr<IInStream> m_cpInStream;
        int m_nSubfileIndex = -1;

        std::weak_ptr<Archive> m_wpArchive;
        std::shared_ptr<IInArchive> m_cpInArchive;
        std::shared_ptr<IArchiveOpenCallback> m_cpCallback;

        void __OpenFile();
        void __OpenStream();
        void __Close();
    };
}

#endif
