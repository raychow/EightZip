#include "Archive.h"

#include "ArchiveEntry.h"
#include "ArchiveFile.h"
#include "Codecs.h"
#include "CommonHelper.h"
#include "IArchive.h"
#include "IArchiveAdapter.h"

using namespace std;

namespace SevenZipCore
{
    Archive::Archive(std::shared_ptr<Codecs> cpCodecs)
        : m_cpCodecs(cpCodecs)
    {
    }

    Archive::~Archive()
    {
        Close();
    }

    void Archive::Open(
        TString tstrPath,
        shared_ptr<IArchiveOpenCallback> cpCallback)
    {
        Open(move(tstrPath), nullptr, move(cpCallback));
    }

    void Archive::Open(
        TString tstrPath,
        shared_ptr<IInStream> cpStream,
        shared_ptr<IArchiveOpenCallback> cpCallback)
    {
        if (!m_vspArchiveEntry.empty())
        {
            throw ArchiveException(
                "The archive class is already associated with a file.");
        }
        m_vspArchiveEntry.clear();

        m_vspArchiveEntry.push_back(make_shared<ArchiveEntry>(
            shared_from_this(),
            m_cpCodecs,
            tstrPath,
            cpStream,
            -1,
            move(cpCallback)));
        // Maybe open inner main stream in the PE file automatically (like 7zFM).

        try
        {
            // TODO: Use InFile::GetTime instead.
            m_oftModified = Helper::GetFileModifiedTime(tstrPath);
        }
        catch (const SystemException &)
        {
        }

        m_tstrPath = move(tstrPath);
        m_spRootFolder = GetArchiveEntry()->GetRootFolder();
    }

    void Archive::Close()
    {
        m_tstrPath.clear();
        m_vspArchiveEntry.clear();
        m_spRootFolder.reset();
    }

    std::shared_ptr<ArchiveEntry> Archive::GetArchiveEntry() const
    {
        if (m_vspArchiveEntry.empty())
        {
            return nullptr;
        }
        return m_vspArchiveEntry.back();
    }

}
