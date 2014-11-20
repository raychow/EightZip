#include "stdwx.h"
#include "FolderEntry.h"

#include <future>
#include <thread>

#include "SevenZipCore/ArchiveEntry.h"
#include "SevenZipCore/ComPtr.h"
#include "SevenZipCore/Exception.h"
#include "SevenZipCore/OpenCallback.h"

#include "CodecsLoader.h"
#include "FileHelper.h"
#include "FolderModel.h"
#include "OpenIndicator.h"
#include "ProgressDialog.h"
#include "VirtualModel.h"

using namespace std;

FolderEntry::FolderEntry(TString tstrLocation,
    TString tstrName,
    bool isDirectory,
    wxULongLong_t un64Size,
    wxDateTime dtAccessed,
    wxDateTime dtModified,
    wxDateTime dtCreated)
    : EntryBase(move(tstrLocation), move(tstrName), isDirectory, false)
    , m_un64Size(un64Size)
    , m_dtAccessed(dtAccessed)
    , m_dtModified(dtModified)
    , m_dtCreated(dtCreated)
{

}

shared_ptr<ModelBase> FolderEntry::GetContainer() const
{
    return make_shared<FolderModel>(GetLocation());
}

shared_ptr<ModelBase> FolderEntry::GetModel() const
{
    auto tstrPath = GetPath();
    if (IsDirectory())
    {
        return make_shared<FolderModel>(tstrPath);
    }
    else
    {
        auto pProgressDialog = new ProgressDialog { wxTheApp->GetTopWindow(),
            wxID_ANY,
            ProgressDialog::Mode::Open };
        auto openIndicator = OpenIndicator { pProgressDialog };
        auto result = promise < shared_ptr < ModelBase > > {};
        thread { [&]() {
            try
            {
                ProgressDialogManager dialogManager(pProgressDialog);
                auto upCallback = SevenZipCore::MakeUniqueCom(
                    new SevenZipCore::OpenCallback { &openIndicator });
                result.set_value(make_shared<VirtualModel>(
                    Helper::GetLocation(tstrPath),
                    SevenZipCore::Helper::GetFileName(tstrPath),
                    tstrPath,
                    nullptr,
                    upCallback.get()));
            }
            catch (...)
            {
                result.set_exception(current_exception());
            }
        } }.detach();
        auto resultFuture = result.get_future();
        while (resultFuture.wait_for(
            chrono::milliseconds(50)) != future_status::ready)
        {
            wxYield();
        }
        return resultFuture.get();
    }
}

bool FolderEntry::CanExtract() const
{
    if (IsDirectory())
    {
        return false;
    }
    else
    {
        auto tstrPath = GetPath();
        try
        {
            SevenZipCore::Archive(CodecsLoader::GetInstance().GetCodecs(),
                GetPath(), nullptr);
            return true;
        }
        catch (const SevenZipCore::SevenZipCoreException &)
        {
            return false;
        }
    }
}

TString FolderEntry::GetItem(EntryItemType itemType) const
{
    switch (itemType)
    {
    case EntryItemType::Size:
        if (!IsDirectory())
        {
            return ToTString(m_un64Size);
        }
        return wxEmptyString;
    case EntryItemType::Modified:
        return m_dtModified.FormatISOCombined(' ').ToStdWstring();
    case EntryItemType::Created:
        return m_dtCreated.FormatISOCombined(' ').ToStdWstring();
    case EntryItemType::Accessed:
        return m_dtAccessed.FormatISOCombined(' ').ToStdWstring();
    }
    return EntryBase::GetItem(itemType);
}

bool FolderEntry::Compare(const EntryBase &otherEntry,
    EntryItemType itemType,
    bool isAscending) const
{
    auto &otherFolderEntry = dynamic_cast<const FolderEntry &>(otherEntry);
    switch (itemType)
    {
    case EntryItemType::Size:
        return OrderCompare(
            m_un64Size, otherFolderEntry.m_un64Size, isAscending);
    case EntryItemType::Modified:
        return OrderCompare(
            m_dtModified, otherFolderEntry.m_dtModified, isAscending);
    case EntryItemType::Created:
        return OrderCompare(
            m_dtCreated, otherFolderEntry.m_dtCreated, isAscending);
    case EntryItemType::Accessed:
        return OrderCompare(
            m_dtAccessed, otherFolderEntry.m_dtAccessed, isAscending);
    }
    return EntryBase::Compare(otherEntry, itemType, isAscending);
}
