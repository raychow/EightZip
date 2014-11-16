// Extractor.h

#ifndef EXTRACTOR_H
#define EXTRACTOR_H

#include <map>
#include <memory>
#include <vector>

#include "SevenZipCore/BaseType.h"
#include "SevenZipCore/TString.h"

#include "ExtractIndicator.h"

namespace SevenZipCore
{
    class ArchiveEntry;
}

class ProgressDialog;
class VirtualEntry;
class VirtualModel;

class Extractor
{
public:
    Extractor(TString tstrExtractLocation,
        ExtractIndicator *pExtractIndicator,
        TString tstrInternalLocation)
        : m_tstrExtractLocation(tstrExtractLocation)
        , m_pExtractIndicator(pExtractIndicator)
        , m_pProgressDialog(pExtractIndicator
        ? pExtractIndicator->GetProgressDialog()
        : nullptr)
        , m_tstrInternalLocation(tstrInternalLocation)
    {

    }
    virtual ~Extractor() = 0 {};

    virtual Extractor &Execute() = 0;

    inline TString GetExtractLocation() const
    {
        return m_tstrExtractLocation;
    }

    inline ExtractIndicator *GetExtractIndicator() const
    {
        return m_pExtractIndicator;
    }

    inline ProgressDialog *GetProgressDialog() const
    {
        return m_pProgressDialog;
    }

    inline const TString &GetLastExtractPath() const
    {
        return m_tstrLastExtractPath;
    }

protected:
    void _Execute(const SevenZipCore::ArchiveEntry &archiveEntry,
        const std::vector<UINT32> &vun32ArchiveIndex);

private:
    TString m_tstrExtractLocation;
    TString m_tstrInternalLocation;
    ExtractIndicator *m_pExtractIndicator = nullptr;
    ProgressDialog *m_pProgressDialog = nullptr;

    TString m_tstrLastExtractPath;

};

class RealFileExtractor
    : public Extractor
{
public:
    RealFileExtractor(
        TString tstrExtractLocation, ExtractIndicator *pExtractIndicator)
        : Extractor(tstrExtractLocation, pExtractIndicator, wxEmptyString)
    {

    }

    inline RealFileExtractor &AddPlan(TString tstrPath)
    {
        m_vtstrPath.push_back(move(tstrPath));
        return *this;
    }

    virtual RealFileExtractor &Execute() override;

private:
    std::vector<TString> m_vtstrPath;

};

class VirtualFileExtractor
    : public Extractor
{
public:
    VirtualFileExtractor(
        TString tstrExtractLocation,
        ExtractIndicator *pExtractIndicator,
        TString tstrInternalLocation,
        TString tstrVirtualArchivePath,
        const SevenZipCore::ArchiveEntry &archiveEntry)
        : Extractor(tstrExtractLocation, pExtractIndicator, tstrInternalLocation)
        , m_tstrVirtualArchivePath(tstrVirtualArchivePath)
        , m_archiveEntry(archiveEntry)
    {

    }

    inline VirtualFileExtractor &AddPlan(UINT32 vun32Index)
    {
        m_vun32Index.push_back(vun32Index);
    }
    inline VirtualFileExtractor &AddPlan(const std::vector<UINT32> &vun32Index)
    {
        m_vun32Index.insert(
            m_vun32Index.end(), vun32Index.cbegin(), vun32Index.cend());
    }
    VirtualFileExtractor &AddPlan(const VirtualEntry &entry);
    VirtualFileExtractor &AddPlan(const VirtualModel &model);

    virtual VirtualFileExtractor &Execute() override;

private:
    TString m_tstrVirtualArchivePath;
    const SevenZipCore::ArchiveEntry &m_archiveEntry;
    std::vector<UINT32> m_vun32Index;

};

#endif // EXTRACTOR_H
