// ArchiveFile.h

#ifndef SEVENZIPCORE_ARCHIVEFILE_H
#define SEVENZIPCORE_ARCHIVEFILE_H

#include <memory>
#include <vector>

#include <boost/optional.hpp>

#include "IArchive.h"
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
        ArchiveFile(boost::optional<UINT> ounIndex, TString tstrName, std::shared_ptr<IInArchive> wpArchive, std::weak_ptr<ArchiveFolder> wpParent);

        boost::optional<UINT> GetIndex() const { return m_ounIndex; }
        void SetIndex(boost::optional<UINT> value) { m_ounIndex = value; }

        const TString &GetName() const { return m_tstrName; }
        void SetName(TString value) { m_tstrName = move(value); }

        std::weak_ptr<ArchiveFolder> GetParent() const { return m_wpParent; }
        void SetParent(std::weak_ptr<ArchiveFolder> value) { m_wpParent = move(value); }

        virtual UINT64 GetSize() const;
        virtual UINT64 GetPackedSize() const;
        virtual boost::optional<UINT32> GetCRC() const;

        void Calculate() const;
    protected:
        boost::optional<UINT> m_ounIndex;
        TString m_tstrName;
        std::shared_ptr<IInArchive> m_spArchive;
        std::weak_ptr<ArchiveFolder> m_wpParent;

        std::unique_ptr<ArchiveInformation> m_upInformation = std::unique_ptr<ArchiveInformation>(new ArchiveInformation());

        virtual void _Calculate() const;
    };

    class ArchiveFolder
        : public ArchiveFile
    {
    public:
        ArchiveFolder(TString tstrName, std::shared_ptr<IInArchive> spArchive, std::weak_ptr<ArchiveFolder> wpParent = std::weak_ptr<ArchiveFolder>());
        ArchiveFolder(boost::optional<UINT> ounIndex, TString tstrName, std::shared_ptr<IInArchive> spArchive, std::weak_ptr<ArchiveFolder> wpParent = std::weak_ptr<ArchiveFolder>());

        std::shared_ptr<ArchiveFolder> AddFolder(TString tstrName, std::weak_ptr<ArchiveFolder> wpParent = std::weak_ptr<ArchiveFolder>());
        std::shared_ptr<ArchiveFolder> AddFolder(UINT unIndex, TString tstrName, std::weak_ptr<ArchiveFolder> wpParent = std::weak_ptr<ArchiveFolder>());
        std::shared_ptr<ArchiveFile> AddFile(UINT unIndex, TString tstrName, std::weak_ptr<ArchiveFolder> wpParent);

        int GetPosition() const { return m_upInformation->Position; }
        void SetPosition(int nPosition) { m_upInformation->Position = nPosition; }

        const std::vector<std::shared_ptr<ArchiveFolder>> &GetFolders() const { return m_vspFolder; }
        const std::vector<std::shared_ptr<ArchiveFile>> &GetFiles() const { return m_vspFile; }

        UINT GetSubFolderCount();
        UINT GetSubFileCount();

        bool IsRealFolder() const { return m_ounIndex; }

    protected:
        bool m_isInformationValid = false;

        std::vector<std::shared_ptr<ArchiveFolder>> m_vspFolder;
        std::vector<std::shared_ptr<ArchiveFile>> m_vspFile;

        void _Calculate() const;

        std::shared_ptr<ArchiveFolder> __AddFolder(boost::optional<UINT> ounIndex, TString tstrName, std::weak_ptr<ArchiveFolder> wpParent = std::weak_ptr<ArchiveFolder>());
    };
}

#endif // SEVENZIPCORE_ARCHIVEFILE_H
