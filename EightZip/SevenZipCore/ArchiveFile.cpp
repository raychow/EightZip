#include "ArchiveFile.h"

#include "Exception.h"
#include "IArchiveAdapter.h"
#include "Property.h"

#include <cassert>
#include <queue>

using namespace boost;
using namespace std;

namespace SevenZipCore
{
    ArchiveFile::ArchiveFile(UINT unIndex,
        TString tstrName,
        ArchiveEntry &archiveEntry,
        boost::optional<ArchiveFolder &> oParent/* = boost::none*/)
        : m_unIndex(unIndex)
        , m_tstrName(move(tstrName))
        , m_archiveEntry(archiveEntry)
        , m_oParent(oParent)
    {
    }

    UINT64 ArchiveFile::GetSize() const
    {
        _CheckCalculate();
        return m_un64Size;
    }

    UINT64 ArchiveFile::GetPackedSize() const
    {
        _CheckCalculate();
        return m_un64PackedSize;
    }

    const boost::optional<UINT32> &ArchiveFile::GetCRC() const
{
        _CheckCalculate();
        return m_oun32CRC;
    }

    void ArchiveFile::_Calculate() const
    {
        if (m_unIndex == UINT_MAX)
        {
            return;
        }
        IInArchiveAdapter<> inArchiveAdapter(
            m_archiveEntry.GetInArchive());
        m_un64Size = PropertyHelper::GetConvertedUInt64(
            inArchiveAdapter.GetProperty(m_unIndex, PropertyId::Size), 0);
        m_un64PackedSize = PropertyHelper::GetConvertedUInt64(
            inArchiveAdapter.GetProperty(m_unIndex, PropertyId::PackSize), 0);
        auto property = inArchiveAdapter.GetProperty(
            m_unIndex, PropertyId::CRC);
        try
        {
            m_oun32CRC = PropertyHelper::GetUInt32(property);
        }
        catch (const PropertyException &)
        {
            m_oun32CRC = none;

        }
    }

    ArchiveFolder::ArchiveFolder(TString tstrName,
        ArchiveEntry &archiveEntry,
        boost::optional<ArchiveFolder &> oParent/* = boost::none*/)
        : ArchiveFolder(UINT_MAX, tstrName, archiveEntry, oParent)
    {
    }

    ArchiveFolder::ArchiveFolder(UINT unIndex,
        TString tstrName,
        ArchiveEntry &archiveEntry,
        boost::optional<ArchiveFolder &> oParent/* = boost::none*/)
        : ArchiveFile(unIndex, tstrName, archiveEntry, oParent)
    {
    }

    ArchiveFolder &ArchiveFolder::AddFolder(TString tstrName,
        boost::optional<ArchiveFolder &> oParent/* = boost::none*/)
    {
        return __AddFolder(UINT_MAX, tstrName, oParent);
    }

    ArchiveFolder &ArchiveFolder::AddFolder(UINT unIndex,
        TString tstrName,
        boost::optional<ArchiveFolder &> oParent/* = boost::none*/)
    {
        return __AddFolder(unIndex, tstrName, oParent);
    }

    ArchiveFile &ArchiveFolder::AddFile(UINT unIndex,
        TString tstrName,
        boost::optional<ArchiveFolder &> oParent/* = boost::none*/)
    {
        m_vupFile.push_back(make_unique<ArchiveFile>(
            unIndex, move(tstrName),
            GetArchiveEntry(),
            oParent));
        _Invalid();
        return *m_vupFile.back();
    }

    vector<UINT32> ArchiveFolder::GetAllIndexes() const
    {
        _CheckCalculate();
        vector<UINT32> result;
        queue<const ArchiveFolder *> qpFolder;
        qpFolder.push(this);

        while (!qpFolder.empty())
        {
            const auto &folder = *qpFolder.front();
            qpFolder.pop();
            for (const auto &spFile : folder.GetFiles())
            {
                result.push_back(spFile->GetIndex());
            }
            for (const auto &spFolder : folder.GetFolders())
            {
                auto un32ArchiveIndex = spFolder->GetIndex();
                if (UINT32_MAX != un32ArchiveIndex)
                {
                    result.push_back(un32ArchiveIndex);
                }
                qpFolder.push(spFolder.get());
            }
        }
        return result;
    }

    void ArchiveFolder::_Calculate() const
    {
        m_un64Size = 0;
        m_un64PackedSize = 0;
        m_oun32CRC = 0;
        for (const auto &file : m_vupFile)
        {
            m_un64Size += file->GetSize();
            m_un64PackedSize += file->GetPackedSize();
            if (m_oun32CRC)
            {
                try
                {
                    if (auto oun32CRC = file->GetCRC())
                    {
                        *m_oun32CRC += *oun32CRC;
                        continue;
                    }
                }
                catch (const SevenZipCoreException &)
                {
                }
                m_oun32CRC.reset();
            }
        }
        for (int i = 0; i != m_vupFolder.size(); ++i)
        {
            const auto &folder = m_vupFolder[i];
            folder->SetPosition(i);
            m_un64Size += folder->GetSize();
            m_un64PackedSize += folder->GetPackedSize();
            if (m_oun32CRC)
            {
                if (const auto &oun32CRC = folder->GetCRC())
                {
                    *m_oun32CRC += *oun32CRC;
                }
                else
                {
                    m_oun32CRC.reset();
                }
            }
        }
    }

    ArchiveFolder &ArchiveFolder::__AddFolder(UINT unIndex,
        TString tstrName,
        boost::optional<ArchiveFolder &> oParent)
    {
        auto iter = lower_bound(
            m_vupFolder.begin(),
            m_vupFolder.end(),
            tstrName,
            [](const unique_ptr<SevenZipCore::ArchiveFolder> &leftFolder,
            const TString &rightTString) {
            return leftFolder->GetName() < rightTString;
        });
        if (iter == m_vupFolder.end() || (*iter)->GetName() != tstrName)
        {
            iter = m_vupFolder.insert(iter, make_unique<ArchiveFolder>(
                unIndex, move(tstrName), m_archiveEntry, oParent));
        }
        if (unIndex != UINT_MAX)
        {
            (*iter)->SetIndex(unIndex);
        }
        _Invalid();
        return **iter;
    }

}
