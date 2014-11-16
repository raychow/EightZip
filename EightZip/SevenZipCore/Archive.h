// Archive.h

#ifndef SEVENZIPCORE_ARCHIVE_H
#define SEVENZIPCORE_ARCHIVE_H

#include <memory>
#include <vector>

#include <boost/optional.hpp>

#include "ArchiveFile.h"
#include "Platform.h"
#include "TString.h"

namespace SevenZipCore
{
    class ArchiveEntry;
    class Codecs;
    struct IArchiveOpenCallback;
    struct IInStream;

    // CArchiveLink
    class Archive
        : public std::enable_shared_from_this < Archive >
    {
    public:
        Archive(std::shared_ptr<Codecs> cpCodecs,
            TString tstrPath,
            IArchiveOpenCallback *pCallback)
            : Archive(move(cpCodecs), move(tstrPath), nullptr, pCallback)
        {
        }
        Archive(std::shared_ptr<Codecs> cpCodecs,
            TString tstrPath,
            std::shared_ptr<IInStream> cpStream,
            IArchiveOpenCallback *pCallback)
            : m_cpCodecs(move(cpCodecs))
        {
            __Open(move(tstrPath), move(cpStream), pCallback);
        }
        virtual ~Archive() { }

        inline const TString &GetPath() const { return m_tstrPath; }

        ArchiveEntry &GetArchiveEntry() const;

        inline const boost::optional<FILETIME> &GetModifiedTime() const
        {
            return m_oftModified;
        }

    private:
        std::shared_ptr<Codecs> m_cpCodecs;

        TString m_tstrPath;

        std::vector<std::unique_ptr<ArchiveEntry>> m_vupArchiveEntry;
        mutable std::unique_ptr<ArchiveFolder> m_upRootFolder;
        boost::optional<FILETIME> m_oftModified;

        void __Open(TString tstrPath,
            std::shared_ptr<IInStream> cpStream,
            IArchiveOpenCallback *pCallback);

    };
}

#endif // SEVENZIPCORE_ARCHIVE_H