// ArchiveFile.h

#ifndef SEVENZIPCORE_ARCHIVEFILE_H
#define SEVENZIPCORE_ARCHIVEFILE_H

#include <memory>
#include <vector>

#include <boost/optional.hpp>

#include "ArchiveEntry.h"
#include "Property.h"
#include "TString.h"

namespace SevenZipCore
{
    class ArchiveFolder;

    struct ArchiveInformation
    {
        bool IsValid = false;
        // Position is used in ArchiveFolder.
        int Position = -1;
        UINT64 Size = 0;
        UINT64 PackedSize = 0;
        boost::optional<UINT32> CRC;
    };

    class ArchiveFile
    {
    public:
        ArchiveFile() {};
        ArchiveFile(
            UINT unIndex,
            TString tstrName,
            std::weak_ptr<ArchiveEntry> wpArchiveEntry,
            std::weak_ptr<ArchiveFolder> wpParent);
        virtual ~ArchiveFile() {}

        inline const UINT GetIndex() const { return m_unIndex; }
        inline void SetIndex(UINT value) { m_unIndex = value; }

        inline const TString &GetName() const { return m_tstrName; }
        inline void SetName(TString value) { m_tstrName = move(value); }

        inline std::shared_ptr<ArchiveFolder> GetParent() const
        {
            return m_wpParent.lock();
        }
        inline void SetParent(std::weak_ptr<ArchiveFolder> value)
        {
            m_wpParent = move(value);
        }

        inline std::shared_ptr<ArchiveEntry> GetArchiveEntry() const
        {
            return m_wpArchiveEntry.lock();
        }

        virtual UINT64 GetSize() const;
        virtual UINT64 GetPackedSize() const;
        virtual const boost::optional<UINT32> &GetCRC() const;

        void Calculate() const;
    protected:
        UINT m_unIndex;
        TString m_tstrName;
        std::weak_ptr<ArchiveEntry> m_wpArchiveEntry;
        std::weak_ptr<ArchiveFolder> m_wpParent;

        std::unique_ptr<ArchiveInformation> m_upInformation
            = std::unique_ptr<ArchiveInformation>(new ArchiveInformation());

        virtual void _Calculate() const;
    };

    class ArchiveFolder
        : public ArchiveFile
    {
    public:
        ArchiveFolder(
            TString tstrName,
            std::weak_ptr<ArchiveEntry> wpArchiveEntry,
            std::weak_ptr<ArchiveFolder> wpParent
            = std::weak_ptr<ArchiveFolder>());
        ArchiveFolder(
            UINT unIndex,
            TString tstrName,
            std::weak_ptr<ArchiveEntry> wpArchiveEntry,
            std::weak_ptr<ArchiveFolder> wpParent
            = std::weak_ptr<ArchiveFolder>());
        virtual ~ArchiveFolder() {}

        std::shared_ptr<ArchiveFolder> AddFolder(
            TString tstrName,
            std::weak_ptr<ArchiveFolder> wpParent
            = std::weak_ptr<ArchiveFolder>());
        std::shared_ptr<ArchiveFolder> AddFolder(
            UINT unIndex,
            TString tstrName,
            std::weak_ptr<ArchiveFolder> wpParent
            = std::weak_ptr<ArchiveFolder>());
        std::shared_ptr<ArchiveFile> AddFile(
            UINT unIndex,
            TString tstrName,
            std::weak_ptr<ArchiveFolder> wpParent);

        inline int GetPosition() const { return m_upInformation->Position; }
        inline void SetPosition(int nPosition)
        {
            m_upInformation->Position = nPosition;
        }

        inline const std::vector<std::shared_ptr<ArchiveFolder>> &GetFolders() const
        {
            return m_vspFolder;
        }
        inline const std::vector<std::shared_ptr<ArchiveFile>> &GetFiles() const
        {
            return m_vspFile;
        }

        UINT GetSubFolderCount();
        UINT GetSubFileCount();

        inline bool IsRealFolder() const { return m_unIndex != UINT_MAX; }

    protected:
        bool m_isInformationValid = false;

        std::vector<std::shared_ptr<ArchiveFolder>> m_vspFolder;
        std::vector<std::shared_ptr<ArchiveFile>> m_vspFile;

        void _Calculate() const;

        std::shared_ptr<ArchiveFolder> __AddFolder(
            UINT unIndex,
            TString tstrName,
            std::weak_ptr<ArchiveFolder> wpParent);
    };
}

#endif // SEVENZIPCORE_ARCHIVEFILE_H
