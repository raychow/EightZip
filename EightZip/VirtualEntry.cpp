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
    shared_ptr<SevenZipCore::ArchiveFile> spArchiveFile)
    : EntryBase(move(tstrLocation), move(tstrName), isDirectory, true)
    , m_un64Size(spArchiveFile->GetSize())
    , m_un64PackedSize(spArchiveFile->GetPackedSize())
    , m_oun32CRC(spArchiveFile->GetCRC())
    , m_wpParent(move(wpParent))
    , m_spArchiveFile(move(spArchiveFile))
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

shared_ptr<ModelBase> VirtualEntry::GetModel() const
{
    if (auto spParent = m_wpParent.lock())
    {
        if (IsDirectory())
        {
            auto result = make_shared<VirtualModel>(GetLocation(),
                GetInternalLocation(),
                GetName(),
                spParent,
                dynamic_pointer_cast<SevenZipCore::ArchiveFolder>(m_spArchiveFile));
            return result;
        }
        else
        {
            auto spArchiveEntry = m_spArchiveFile->GetArchiveEntry();
            SevenZipCore::IInArchiveAdapter<> inArchiveAdapter(
                spArchiveEntry->GetInArchive());
            try
            {
                auto cpGetStream = inArchiveAdapter.QueryInterface<
                    SevenZipCore::IInArchiveGetStream>(SevenZipCore::IID_IInArchiveGetStream);
                if (cpGetStream)
                {
                    auto cpSubSeqStream = SevenZipCore::IInArchiveGetStreamAdapter<>
                        (cpGetStream).GetStream(m_spArchiveFile->GetIndex());
                    if (cpSubSeqStream)
                    {
                        auto cpSubStream =
                            SevenZipCore::Helper::QueryInterface<
                            SevenZipCore::IInStream>(
                            cpSubSeqStream, SevenZipCore::IID_IInStream);
                        if (cpSubStream)
                        {
                            return make_shared<VirtualModel>(GetLocation(),
                                GetName(),
                                m_wpParent.lock(),
                                cpSubStream,
                                SevenZipCore::MakeComPtr(new SevenZipCore::OpenCallback));
                        }
                    }
                }
            }
            catch (const SevenZipCore::SevenZipCoreException &)
            {
            }
            __ExtractToTempFolder();
            return make_shared<VirtualModel>(GetLocation(),
                GetName(),
                m_upTempFolder->GetFilePath(),
                m_wpParent.lock(),
                SevenZipCore::MakeComPtr(new SevenZipCore::OpenCallback));
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
        Extractor(m_upTempFolder->GetLocation(), nullptr)
        .AddPlan(m_spArchiveFile->GetArchiveEntry(), m_spArchiveFile->GetIndex())
        .SetInternalLocation(m_wpParent.lock()->GetInternalLocation())
        .Execute().GetLastExtractPath());
}