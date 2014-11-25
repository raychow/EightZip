#include "stdwx.h"
#include "VirtualEntry.h"

#include <future>
#include <thread>

#include "SevenZipCore/CommonHelper.h"
#include "SevenZipCore/ComPtr.h"
#include "SevenZipCore/Exception.h"
#include "SevenZipCore/IArchiveAdapter.h"

#include "Extractor.h"
#include "FileHelper.h"
#include "OpenIndicator.h"
#include "ProgressDialog.h"
#include "VirtualModel.h"
#include "VirtualRootModel.h"

using namespace std;

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

shared_ptr<ModelBase> VirtualEntry::GetContainer() const
{
    return m_wpParent.lock();
}

shared_ptr<ModelBase> VirtualEntry::GetModel() const
{
    auto spParent = m_wpParent.lock();
    if (!spParent)
    {
        return nullptr;
    }
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
        auto progressDialog = ProgressDialog { wxTheApp->GetTopWindow(),
            wxID_ANY, ProgressDialog::Mode::Open };
        promise<shared_ptr<ModelBase>> result;
        thread { [&]() {
            try
            {
                result.set_value(__GetVirtualModel(&progressDialog));
            }
            catch (...)
            {
                result.set_exception(current_exception());
            }
            wxTheApp->CallAfter([&]() {
                progressDialog.Done(false);
            });
        } }.detach();
        progressDialog.ShowModal();
        return result.get_future().get();
    }
}

void VirtualEntry::OpenExternal() const
{
    if (!m_upTempFolder)
    {
        __ExtractToTempFolder(nullptr);
    }
    auto tstrFilePath = m_upTempFolder->GetFilePath();
    if (!tstrFilePath.empty())
    {
        Helper::OpenFileExternal(tstrFilePath);
    }
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

void VirtualEntry::__ExtractToTempFolder(ProgressDialog *pProgressDialog) const
{
    m_upTempFolder.reset(new TempFolder());
    m_upTempFolder->SetFilePath(
        VirtualFileExtractor(m_upTempFolder->GetLocation(),
        &dynamic_pointer_cast<VirtualModel>(GetContainer())->GetProperty(),
        pProgressDialog,
        m_wpParent.lock()->GetInternalLocation(),
        GetPath(), GetArchiveFile().GetArchiveEntry())
        .AddPlan(*this)
        .Execute().GetLastExtractPath());
}

shared_ptr<VirtualModel> VirtualEntry::__GetVirtualModel(
    ProgressDialog *pProgressDialog) const
{
    auto &archiveEntry = m_archiveFile.GetArchiveEntry();
    SevenZipCore::IInArchiveAdapter<> inArchiveAdapter(
        archiveEntry.GetInArchive());
    try
    {
        for (;;)
        {
            auto cpGetStream = inArchiveAdapter.QueryInterface <
                SevenZipCore::IInArchiveGetStream > (
                SevenZipCore::IID_IInArchiveGetStream);
            if (!cpGetStream)
            {
                break;
            }
            auto cpSubSeqStream = SevenZipCore::IInArchiveGetStreamAdapter<>
                (*cpGetStream).GetStream(m_archiveFile.GetIndex());
            if (!cpSubSeqStream)
            {
                break;
            }
            auto cpSubStream = SevenZipCore::Helper::QueryInterface <
                SevenZipCore::IInStream > (
                *cpSubSeqStream, SevenZipCore::IID_IInStream);
            if (!cpSubStream)
            {
                break;
            }
            return make_shared<VirtualRootModel>(GetLocation(),
                GetName(),
                m_wpParent.lock(),
                move(cpSubStream),
                pProgressDialog);
            break;
        }
    }
    catch (const SevenZipCore::SevenZipCoreException &)
    {
    }
    __ExtractToTempFolder(pProgressDialog);
    return make_shared<VirtualRootModel>(GetLocation(),
        GetName(),
        m_upTempFolder->GetFilePath(),
        m_wpParent.lock(),
        pProgressDialog);
}
