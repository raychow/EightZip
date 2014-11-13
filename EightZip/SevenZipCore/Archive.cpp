#include "Archive.h"

#include "ArchiveEntry.h"
#include "Codecs.h"
#include "CommonHelper.h"
#include "IArchive.h"
#include "IArchiveAdapter.h"

using namespace std;

namespace SevenZipCore
{
    ArchiveEntry &Archive::GetArchiveEntry() const
    {
        return *m_vupArchiveEntry.back();
    }

    void Archive::__Open(TString tstrPath,
        shared_ptr<IInStream> cpStream,
        IArchiveOpenCallback &callback)
    {
        m_vupArchiveEntry.push_back(make_unique<ArchiveEntry>(*this,
            m_cpCodecs,
            tstrPath,
            cpStream,
            -1,
            callback));
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
    }

}
