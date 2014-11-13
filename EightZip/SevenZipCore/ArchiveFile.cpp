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
    ArchiveFile::ArchiveFile(
        UINT unIndex,
        TString tstrName,
        weak_ptr<ArchiveEntry> wpArchiveEntry,
        weak_ptr<ArchiveFolder> wpParent)
        : m_unIndex(unIndex)
        , m_tstrName(move(tstrName))
        , m_wpArchiveEntry(move(wpArchiveEntry))
        , m_wpParent(move(wpParent))
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
            m_wpArchiveEntry.lock()->GetInArchive());
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

    ArchiveFolder::ArchiveFolder(
        TString tstrName,
        weak_ptr<ArchiveEntry> wpArchiveEntry,
        weak_ptr<ArchiveFolder> wpParent /*= nullptr*/)
        : ArchiveFolder(UINT_MAX, tstrName, wpArchiveEntry, wpParent)
    {
    }

    ArchiveFolder::ArchiveFolder(
        UINT unIndex,
        TString tstrName,
        weak_ptr<ArchiveEntry> wpArchiveEntry,
        weak_ptr<ArchiveFolder> wpParent /*= nullptr*/)
        : ArchiveFile(unIndex, tstrName, wpArchiveEntry, wpParent)
    {
    }

    shared_ptr<ArchiveFolder> ArchiveFolder::AddFolder(
        TString tstrName,
        weak_ptr<ArchiveFolder> wpParent /*= nullptr*/)
    {
        return __AddFolder(UINT_MAX, tstrName, wpParent);
    }

    shared_ptr<ArchiveFolder> ArchiveFolder::AddFolder(
        UINT unIndex,
        TString tstrName,
        weak_ptr<ArchiveFolder> wpParent /*= nullptr*/)
    {
        return __AddFolder(unIndex, tstrName, wpParent);
    }

    shared_ptr<ArchiveFile> ArchiveFolder::AddFile(
        UINT unIndex,
        TString tstrName,
        weak_ptr<ArchiveFolder> wpParent)
    {
        auto spFile = make_shared<ArchiveFile>(
            unIndex, move(tstrName),
            GetArchiveEntry(),
            move(wpParent));
        m_vspFile.push_back(move(spFile));
        _Invalid();
        return spFile;
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
        for (const auto &file : m_vspFile)
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
        for (int i = 0; i != m_vspFolder.size(); ++i)
        {
            const auto &folder = m_vspFolder[i];
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

    shared_ptr<ArchiveFolder> ArchiveFolder::__AddFolder(
        UINT unIndex,
        TString tstrName,
        weak_ptr<ArchiveFolder> wpParent /*= weak_ptr<ArchiveFolder>()*/)
    {
        auto iter = lower_bound(
            m_vspFolder.begin(),
            m_vspFolder.end(),
            tstrName,
            [](const shared_ptr<SevenZipCore::ArchiveFolder> &leftFolder,
            const TString &rightTString) {
            return leftFolder->GetName() < rightTString;
        });
        if (iter == m_vspFolder.end() || (*iter)->GetName() != tstrName)
        {
            auto spFolder = make_shared<ArchiveFolder>(
                unIndex, move(tstrName), m_wpArchiveEntry, move(wpParent));
            iter = m_vspFolder.insert(iter, move(spFolder));
        }
        if (unIndex != UINT_MAX)
        {
            (*iter)->SetIndex(unIndex);
        }
        _Invalid();
        return *iter;
    }

}
