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

    class ArchiveFile
    {
    public:
        ArchiveFile(UINT unIndex,
            TString tstrName,
            ArchiveEntry &archiveEntry,
            boost::optional<ArchiveFolder &> oParent = boost::none);
        virtual ~ArchiveFile() {}

        inline const UINT GetIndex() const { return m_unIndex; }
        inline void SetIndex(UINT value) { m_unIndex = value; }

        inline const TString &GetName() const { return m_tstrName; }
        inline void SetName(TString value) { m_tstrName = move(value); }

        inline boost::optional<ArchiveFolder &> GetParent() const
        {
            return m_oParent;
        }
        inline void ResetParent()
        {
            m_oParent.reset();
        }
        inline void SetParent(ArchiveFolder &value)
        {
            m_oParent.reset(value);
        }

        inline ArchiveEntry &GetArchiveEntry() const
        {
            return m_archiveEntry;
        }

        virtual UINT64 GetSize() const;
        virtual UINT64 GetPackedSize() const;
        virtual const boost::optional<UINT32> &GetCRC() const;

    protected:
        mutable UINT64 m_un64Size = 0;
        mutable UINT64 m_un64PackedSize = 0;
        mutable boost::optional<UINT32> m_oun32CRC;
        ArchiveEntry &m_archiveEntry;
        boost::optional<ArchiveFolder &> m_oParent;

        inline void _CheckCalculate() const
        {
            if (!m_isValid)
            {
                _Calculate();
                m_isValid = true;
            }
        }
        inline void _Invalid() const { m_isValid = false; }
        virtual void _Calculate() const;

    private:
        UINT m_unIndex;
        TString m_tstrName;

        mutable bool m_isValid = false;

    };

    class ArchiveFolder
        : public ArchiveFile
    {
    public:
        ArchiveFolder(TString tstrName,
            ArchiveEntry &archiveEntry,
            boost::optional<ArchiveFolder &> oParent = boost::none);
        ArchiveFolder(UINT unIndex,
            TString tstrName,
            ArchiveEntry &archiveEntry,
            boost::optional<ArchiveFolder &> oParent = boost::none);
        virtual ~ArchiveFolder() {}

        ArchiveFolder &AddFolder(TString tstrName,
            boost::optional<ArchiveFolder &> oParent = boost::none);
        ArchiveFolder &AddFolder(UINT unIndex,
            TString tstrName,
            boost::optional<ArchiveFolder &> oParent = boost::none);
        ArchiveFile &AddFile(UINT unIndex,
            TString tstrName,
            boost::optional<ArchiveFolder &> oParent = boost::none);

        inline int GetPosition() const { return m_nPosition; }
        inline void SetPosition(int nPosition)
        {
            m_nPosition = nPosition;
        }

        inline const std::vector<std::unique_ptr<ArchiveFolder>> &GetFolders() const
        {
            return m_vupFolder;
        }
        inline const std::vector<std::unique_ptr<ArchiveFile>> &GetFiles() const
        {
            return m_vupFile;
        }

        inline bool IsRealFolder() const { return GetIndex() != UINT_MAX; }

        std::vector<UINT32> GetAllIndexes() const;

    protected:
        virtual void _Calculate() const;

    private:
        mutable int m_nPosition = -1;

        std::vector<std::unique_ptr<ArchiveFolder>> m_vupFolder;
        std::vector<std::unique_ptr<ArchiveFile>> m_vupFile;

        ArchiveFolder &__AddFolder(UINT unIndex,
            TString tstrName,
            boost::optional<ArchiveFolder &> oParent);

    };
}

#endif // SEVENZIPCORE_ARCHIVEFILE_H
