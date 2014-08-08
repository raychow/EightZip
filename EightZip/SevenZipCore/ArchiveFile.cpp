#include "ArchiveFile.h"

#include "Exception.h"
#include "IArchiveAdapter.h"
#include "Property.h"

#include <cassert>

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
        , m_upInformation(new ArchiveInformation())
    {
    }

    UINT64 ArchiveFile::GetSize() const
    {
        _Calculate();
        return m_upInformation->Size;
    }

    UINT64 ArchiveFile::GetPackedSize() const
    {
        _Calculate();
        return m_upInformation->PackedSize;
    }

    const boost::optional<UINT32> &ArchiveFile::GetCRC() const
{
        _Calculate();
        return m_upInformation->CRC;
    }

    void ArchiveFile::Calculate() const
    {
        m_upInformation->IsValid = false;
        _Calculate();
    }

    void ArchiveFile::_Calculate() const
    {
        if (m_unIndex == UINT_MAX || m_upInformation->IsValid)
        {
            return;
        }
        IInArchiveAdapter inArchiveAdapter(
            m_wpArchiveEntry.lock()->GetInArchive());
        m_upInformation->Size = PropertyHelper::GetConvertedUInt64(
            inArchiveAdapter.GetProperty(m_unIndex, PropertyId::Size), 0);
        m_upInformation->PackedSize = PropertyHelper::GetConvertedUInt64(
            inArchiveAdapter.GetProperty(m_unIndex, PropertyId::PackSize), 0);
        auto property = inArchiveAdapter.GetProperty(
            m_unIndex, PropertyId::CRC);
        try
        {
            m_upInformation->CRC = PropertyHelper::GetUInt32(property);
        }
        catch (const PropertyException &)
        {
            m_upInformation->CRC = none;

        }
        m_upInformation->IsValid = true;
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
        m_isInformationValid = false;
        auto spFile = make_shared<ArchiveFile>(
            unIndex, move(tstrName),
            m_wpArchiveEntry,
            move(wpParent));
        m_vspFile.push_back(move(spFile));
        return spFile;
    }

    void ArchiveFolder::_Calculate() const
    {
        if (m_upInformation->IsValid)
        {
            return;
        }
        m_upInformation->Size = 0;
        m_upInformation->PackedSize = 0;
        m_upInformation->CRC = 0;
        for (const auto &file : m_vspFile)
        {
            m_upInformation->Size += file->GetSize();
            m_upInformation->PackedSize += file->GetPackedSize();
            if (m_upInformation->CRC)
            {
                try
                {
                    if (auto oun32CRC = file->GetCRC())
                    {
                        *m_upInformation->CRC += *oun32CRC;
                        continue;
                    }
                }
                catch (const SevenZipCoreException &)
                {
                }
                m_upInformation->CRC.reset();
            }
        }
        for (int i = 0; i != m_vspFolder.size(); ++i)
        {
            const auto &folder = m_vspFolder[i];
            folder->SetPosition(i);
            m_upInformation->Size += folder->GetSize();
            m_upInformation->PackedSize += folder->GetPackedSize();
            if (m_upInformation->CRC)
            {
                if (const auto &oun32CRC = folder->GetCRC())
                {
                    *m_upInformation->CRC += *oun32CRC;
                }
                else
                {
                    m_upInformation->CRC.reset();
                }
            }
        }
        m_upInformation->IsValid = true;
    }

    shared_ptr<ArchiveFolder> ArchiveFolder::__AddFolder(
        UINT unIndex,
        TString tstrName,
        weak_ptr<ArchiveFolder> wpParent /*= weak_ptr<ArchiveFolder>()*/)
    {
        m_isInformationValid = false;
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
        return *iter;
    }

}
