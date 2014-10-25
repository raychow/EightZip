#include "stdwx.h"
#include "ArchiveFileModel.h"

#include "SevenZipCore/CommonHelper.h"
#include "SevenZipCore/IArchiveAdapter.h"

using namespace std;

ArchiveFileModel::ArchiveFileModel(
    std::shared_ptr<ModelBase> spParent,
    TString tstrVirtualLocation,
    TString tstrVirtualFileName,
    TString tstrInternalPath,
    shared_ptr<SevenZipCore::ArchiveFile> spArchiveFile)
    : ModelBase(tstrVirtualLocation, tstrVirtualFileName, false, true)
    , m_spParent(move(spParent))
    , m_tstrInternalPath(
    SevenZipCore::Helper::MakePathSlash(move(tstrInternalPath)))
    , m_spArchiveFile(move(spArchiveFile))
{

}

shared_ptr<ModelBase> ArchiveFileModel::GetParent() const
{
    return m_spParent;
}

const vector<ModelItemType> &ArchiveFileModel::GetSupportedItems() const
{
    static vector<ModelItemType> vType = {
        ModelItemType::Name,
        ModelItemType::Type,
        ModelItemType::Size,
        ModelItemType::PackedSize,
        //ModelItemType::Modified,
        ModelItemType::CRC
    };
    return vType;
}

bool ArchiveFileModel::CanExtract() const
{
    return true;
}


const ModelBase::Models ArchiveFileModel::GetChildren() const
{
    Models result;
    auto spArchiveEntry = m_spArchiveFile->GetArchiveEntry();
    SevenZipCore::IInArchiveAdapter<> inArchiveAdapter(
        spArchiveEntry->GetInArchive());
    try
    {
        auto cpGetStream = inArchiveAdapter.QueryInterface<
            SevenZipCore::IInArchiveGetStream>(
            SevenZipCore::IID_IInArchiveGetStream);
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
                    auto result = make_shared<ArchiveModel>(
                        m_wpParent.lock(),
                        GetPath(),
                        TString(),
                        cpSubStream,
                        SevenZipCore::MakeComPtr(new SevenZipCore::OpenCallback));
                    return result;
                }
            }
        }
    }
    catch (const SevenZipCore::SevenZipCoreException &)
    {
    }
    __ExtractToTempFolder();
    auto result = make_shared<ArchiveModel>(
        m_wpParent.lock(),
        GetPath(),
        TString(),
        m_upTempFolder->GetFilePath(),
        SevenZipCore::MakeComPtr(new SevenZipCore::OpenCallback));
    return result;
}