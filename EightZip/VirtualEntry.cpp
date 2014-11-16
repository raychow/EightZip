#include "stdwx.h"
#include "VirtualEntry.h"

using namespace std;

#include "SevenZipCore/CommonHelper.h"
#include "SevenZipCore/ComPtr.h"
#include "SevenZipCore/Exception.h"
#include "SevenZipCore/IArchiveAdapter.h"
#include "SevenZipCore/OpenCallback.h"

#include "Extractor.h"
#include "FileHelper.h"
#include "VirtualModel.h"

VirtualEntry::VirtualEntry(TString tstrLocation,
    TString tstrName,
    bool isDirectory,
    weak_ptr<VirtualModel> wpParent,
    SevenZipCore::ArchiveFile &archiveFile)
    : EntryBase(move(tstrLocation), move(tstrName), isDirectory, true)
    , m_un64Size(archiveFile.GetSize())
    , m_un64PackedSize(archiveFile.GetPackedSize())
    , m_oun32CRC(archiveFile.GetCRC())
    , m_wpParent(move(wpParent))
    , m_archiveFile(archiveFile)
{

}

TString VirtualEntry::GetInternalLocation() const
{
    if (auto spParent = m_wpParent.lock())
    {
        return spParent->GetInternalLocation() + GetName() + wxFILE_SEP_PATH;
    }
    return GetName();
}

std::shared_ptr<ModelBase> VirtualEntry::GetContainer() const
{
    return m_wpParent.lock();
}

std::shared_ptr<ModelBase> VirtualEntry::GetModel() const
{
    if (auto spParent = m_wpParent.lock())
    {
        if (IsDirectory())
        {
            return make_shared<VirtualModel>(GetLocation(),
                GetInternalLocation(),
                GetName(),
                spParent,
                dynamic_cast<SevenZipCore::ArchiveFolder &>(m_archiveFile));
        }
        else
        {
            auto &archiveEntry = m_archiveFile.GetArchiveEntry();
            SevenZipCore::IInArchiveAdapter<> inArchiveAdapter(
                archiveEntry.GetInArchive());
            try
            {
                auto cpGetStream = inArchiveAdapter.QueryInterface<
                    SevenZipCore::IInArchiveGetStream>(SevenZipCore::IID_IInArchiveGetStream);
                if (cpGetStream)
                {
                    auto cpSubSeqStream = SevenZipCore::IInArchiveGetStreamAdapter<>
                        (*cpGetStream).GetStream(m_archiveFile.GetIndex());
                    if (cpSubSeqStream)
                    {
                        auto cpSubStream = SevenZipCore::Helper::QueryInterface<
                            SevenZipCore::IInStream>(
                            *cpSubSeqStream, SevenZipCore::IID_IInStream);
                        if (cpSubStream)
                        {
                            auto upCallback = SevenZipCore::MakeUniqueCom(
                                new SevenZipCore::OpenCallback);
                            return make_shared<VirtualModel>(GetLocation(),
                                GetName(),
                                m_wpParent.lock(),
                                move(cpSubStream),
                                upCallback.get());
                        }
                    }
                }
            }
            catch (const SevenZipCore::SevenZipCoreException &)
            {
            }
            __ExtractToTempFolder();
            auto upCallback = SevenZipCore::MakeUniqueCom(
                new SevenZipCore::OpenCallback);
            return make_shared<VirtualModel>(GetLocation(),
                GetName(),
                m_upTempFolder->GetFilePath(),
                m_wpParent.lock(),
                upCallback.get());
        }
    }
    else
    {
        return nullptr;
    }
}

void VirtualEntry::OpenExternal() const
{
    if (!m_upTempFolder)
    {
        __ExtractToTempFolder();
    }
    Helper::OpenFileExternal(m_upTempFolder->GetFilePath());
}

TString VirtualEntry::GetItem(EntryItemType itemType) const
{
    switch (itemType)
    {
    case EntryItemType::Size:
        return ToTString(m_un64Size);
    case EntryItemType::PackedSize:
        return ToTString(m_un64PackedSize);
    case EntryItemType::Modified:
        return m_dtModified.FormatISOCombined(' ').ToStdWstring();
    case EntryItemType::CRC:
        if (m_oun32CRC)
        {
            TStringStream tss;
            tss << hex << setw(8) << setfill(wxT('0'));
            tss << uppercase << *m_oun32CRC;
            return tss.str();
        }
        else
        {
            return wxEmptyString;
        }
    }
    return EntryBase::GetItem(itemType);
}

bool VirtualEntry::Compare(const EntryBase &otherEntry,
    EntryItemType itemType,
    bool isAscending) const
{
    auto &otherArchiveEntry = dynamic_cast<const VirtualEntry &>(otherEntry);
    switch (itemType)
    {
    case EntryItemType::Size:
        return OrderCompare(
            m_un64Size, otherArchiveEntry.m_un64Size, isAscending);
    case EntryItemType::PackedSize:
        return OrderCompare(
            m_un64PackedSize, otherArchiveEntry.m_un64PackedSize, isAscending);
    case EntryItemType::Modified:
        return OrderCompare(
            m_dtModified, otherArchiveEntry.m_dtModified, isAscending);
    case EntryItemType::CRC:
        return OrderCompare(
            m_oun32CRC, otherArchiveEntry.m_oun32CRC, isAscending);
    }
    return EntryBase::Compare(otherEntry, itemType, isAscending);
}

void VirtualEntry::__ExtractToTempFolder() const
{
    m_upTempFolder.reset(new TempFolder());
    m_upTempFolder->SetFilePath(
        VirtualFileExtractor(m_upTempFolder->GetLocation(), nullptr,
        m_wpParent.lock()->GetInternalLocation(),
        wxEmptyString, GetArchiveFile().GetArchiveEntry())
        .AddPlan(*this)
        .Execute().GetLastExtractPath());
}