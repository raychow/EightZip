#include "stdwx.h"
#include "ArchiveFile.h"

#include "Exception.h"
#include "IArchiveAdapter.h"
#include "Property.h"

#include <cassert>

using namespace boost;
using namespace std;

namespace SevenZipCore
{
    ArchiveFile::ArchiveFile(optional<UINT> ounIndex, TString tstrName, shared_ptr<IInArchive> spArchive, std::weak_ptr<ArchiveFolder> wpParent)
        : m_ounIndex(ounIndex)
        , m_tstrName(move(tstrName))
        , m_spArchive(move(spArchive))
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

    optional<UINT32> ArchiveFile::GetCRC() const
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
        if (!m_ounIndex || m_upInformation->IsValid)
        {
            return;
        }
        IInArchiveAdapter archiveAdapter(m_spArchive);
        m_upInformation->Size = PropertyHelper::GetConvertedUInt64(archiveAdapter.GetProperty(*m_ounIndex, PropId::Size));
        m_upInformation->PackedSize = PropertyHelper::GetConvertedUInt64(archiveAdapter.GetProperty(*m_ounIndex, PropId::PackSize));
        auto property = archiveAdapter.GetProperty(*m_ounIndex, PropId::CRC);
        if (property.vt == VT_UI4)
        {
            m_upInformation->CRC = PropertyHelper::GetUInt32(property);
        }
        else
        {
            m_upInformation->CRC = none;
        }
        m_upInformation->IsValid = true;
    }

    ArchiveFolder::ArchiveFolder(TString tstrName, std::shared_ptr<IInArchive> spArchive, std::weak_ptr<ArchiveFolder> wpParent /*= nullptr*/)
        : ArchiveFolder(none, tstrName, spArchive, wpParent)
    {
    }

    ArchiveFolder::ArchiveFolder(boost::optional<UINT> ounIndex, TString tstrName, std::shared_ptr<IInArchive> spArchive, std::weak_ptr<ArchiveFolder> wpParent /*= nullptr*/)
        : ArchiveFile(ounIndex, tstrName, spArchive, wpParent)
    {
    }

    std::shared_ptr<ArchiveFolder> ArchiveFolder::AddFolder(TString tstrName, std::weak_ptr<ArchiveFolder> wpParent /*= nullptr*/)
    {
        return __AddFolder(none, tstrName, wpParent);
    }

    std::shared_ptr<ArchiveFolder> ArchiveFolder::AddFolder(UINT unIndex, TString tstrName, std::weak_ptr<ArchiveFolder> wpParent /*= nullptr*/)
    {
        return __AddFolder(unIndex, tstrName, wpParent);
    }

    std::shared_ptr<ArchiveFile> ArchiveFolder::AddFile(UINT unIndex, TString tstrName, std::weak_ptr<ArchiveFolder> wpParent)
    {
        m_isInformationValid = false;
        auto spFile = make_shared<ArchiveFile>(unIndex, move(tstrName), m_spArchive, move(wpParent));
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
                catch (SevenZipCoreException)
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

    shared_ptr<ArchiveFolder> ArchiveFolder::__AddFolder(boost::optional<UINT> ounIndex, TString tstrName, std::weak_ptr<ArchiveFolder> wpParent /*= std::weak_ptr<ArchiveFolder>()*/)
    {
        m_isInformationValid = false;
        auto iter = lower_bound(m_vspFolder.begin(), m_vspFolder.end(), tstrName, [](const shared_ptr<SevenZipCore::ArchiveFolder> &leftFolder, const TString &rightTString) {
            return leftFolder->GetName() < rightTString;
        });
        if (iter == m_vspFolder.end() || (*iter)->GetName() != tstrName)
        {
            auto spFolder = make_shared<ArchiveFolder>(ounIndex, move(tstrName), m_spArchive, move(wpParent));
            iter = m_vspFolder.insert(iter, move(spFolder));
        }
        if (ounIndex)
        {
            (*iter)->SetIndex(ounIndex);
        }
        return *iter;
    }

}
