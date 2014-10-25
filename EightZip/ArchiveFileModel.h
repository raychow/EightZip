// ArchiveFileModel.h

#ifndef ARCHIVEFILEMODEL_H
#define ARCHIVEFILEMODEL_H

#include "SevenZipCore/ArchiveFile.h"
#include "SevenZipCore/IStream.h"

#include "IExtractable.h"
#include "ModelBase.h"

class ArchiveFileModel
    : public ModelBase
    , public IExtractable
    , public std::enable_shared_from_this < ArchiveFileModel >
{
public:
    // Open stream.
    ArchiveFileModel(
        std::shared_ptr<ModelBase> spParent,
        TString tstrVirtualLocation,
        TString tstrVirtualFileName,
        TString tstrInternalPath,
        std::shared_ptr<SevenZipCore::ArchiveFile> spArchiveFile);

    virtual std::shared_ptr<ModelBase> GetParent() const;
    virtual const std::vector<ModelItemType> &GetSupportedItems() const;
    virtual bool CanExtract() const;
    virtual const Models GetChildren() const;

    virtual void Extract(TString tstrPath,
        SevenZipCore::IExtractIndicator *pExtractIndicator) const;
    virtual TString Extract(UINT32 un32ArchiveIndex, TString tstrPath,
        SevenZipCore::IExtractIndicator *pExtractIndicator) const;
    virtual void Extract(std::vector<UINT32> vun32ArchiveIndex,
        TString tstrPath,
        SevenZipCore::IExtractIndicator *pExtractIndicator) const;

    inline const TString &GetInternalPath() const
    {
        return m_tstrInternalPath;
    }

    virtual ~ArchiveFileModel() { }

private:
    wxULongLong_t m_un64Size = 0;
    wxULongLong_t m_un64PackedSize = 0;
    wxDateTime m_dtModified;
    boost::optional<UINT32> m_oun32CRC;

    std::shared_ptr<SevenZipCore::ArchiveFile> m_spArchiveFile;
    std::shared_ptr<ModelBase> m_spParent;

    TString m_tstrInternalPath;

};

#endif // ARCHIVEFILEMODEL_H
