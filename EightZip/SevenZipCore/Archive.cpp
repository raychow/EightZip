#include "stdwx.h"
#include "Archive.h"

#include "Common.h"

using namespace std;

namespace SevenZipCore
{
    Archive::Archive(std::shared_ptr<Codecs> cpCodecs)
        : m_cpCodecs(cpCodecs)
    {
    }

    Archive::Archive(std::shared_ptr<Codecs> cpCodecs, TString tstrPath, shared_ptr<IArchiveOpenCallback> cpCallback)
        : Archive(cpCodecs)
    {
        Open(move(tstrPath), move(cpCallback));
    }

    Archive::~Archive()
    {
        Close();
    }

    void Archive::Open(TString tstrPath, shared_ptr<IArchiveOpenCallback> cpCallback)
    {
        if (!m_vupArchiveEntry.empty())
        {
            throw ArchiveException("The archive class is already associated with a file.");
        }
        m_vupArchiveEntry.clear();

        // make_unique is not available in C++ 0x.
        m_vupArchiveEntry.push_back(move(unique_ptr<ArchiveEntry>(new ArchiveEntry(*m_cpCodecs, tstrPath, move(cpCallback)))));
        // Maybe open inner main stream in the PE file automatically (like 7zFM).

        m_tstrPath = move(tstrPath);
        __LoadArchiveList();
    }

    void Archive::Close()
    {
        m_tstrPath.clear();
        m_vupArchiveEntry.clear();
        m_spRootFolder.reset();
    }

    void Archive::__LoadArchiveList()
    {
        const auto &archiveEntry = *m_vupArchiveEntry.back();
        auto archive = archiveEntry.GetArchive();
        m_spRootFolder = make_shared<ArchiveFolder>(TEXT(""), archive);
        IInArchiveAdapter archiveAdapter(archive);
        UINT32 un32ItemCount = archiveAdapter.GetNumberOfItems();
        TString tstrArchiveFileName = Helper::GetFileName(m_tstrPath);
        for (UINT32 i = 0; i != un32ItemCount; ++i)
        {
            shared_ptr<ArchiveFolder> spCurrentFolder = m_spRootFolder;
            TString tstrItemPath = archiveAdapter.GetItemPath(i);
            auto vtstrFolder = Helper::SplitString(tstrItemPath, FOLDER_POSSIBLE_SEPARATOR, true);
            const auto tstrBack = vtstrFolder.back();
            vtstrFolder.pop_back();
            for (const auto &strFolder : vtstrFolder)
            {
                if (strFolder.empty())
                {
                    continue;
                }
                spCurrentFolder = spCurrentFolder->AddFolder(strFolder, spCurrentFolder);
            }
            if (tstrBack.empty())
            {
                continue;
            }
            if (PropertyHelper::GetBool(archiveAdapter.GetProperty(i, PropertyId::IsDir), false))
            {
                spCurrentFolder->AddFolder(i, tstrBack, spCurrentFolder);
            }
            else
            {
                spCurrentFolder->AddFile(i, tstrBack, spCurrentFolder);
            }
        }
        m_spRootFolder->Calculate();
    }

}
