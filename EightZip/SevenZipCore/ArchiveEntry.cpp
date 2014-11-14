#include "ArchiveEntry.h"

#include <algorithm>
#include <cassert>
#include <vector>

#include "Archive.h"
#include "ArchiveFile.h"
#include "Codecs.h"
#include "CommonHelper.h"
#include "ComPtr.h"
#include "Exception.h"
#include "FileStream.h"
#include "IArchiveAdapter.h"
#include "ICoderAdapter.h"
#include "IStream.h"

using namespace std;

namespace SevenZipCore
{
    ArchiveEntry::ArchiveEntry(Archive &archive,
        shared_ptr<Codecs> cpCodecs,
        TString tstrPath,
        std::shared_ptr<IInStream> cpStream,
        int nSubFileIndex,
        IArchiveOpenCallback &callback)
        : m_archive(archive)
        , m_cpCodecs(cpCodecs)
        , m_tstrPath(move(tstrPath))
        , m_cpInStream(cpStream)
        , m_nSubfileIndex(nSubFileIndex)
    {
        if (!m_cpInStream)
        {
            __OpenFile(callback);
        }
        __OpenStream(callback);
        __LoadFolder();
    }

    ArchiveEntry::~ArchiveEntry()
    {
        try
        {
            __Close();
        }
        catch (const ArchiveException &)
        {
        }
    }

    void ArchiveEntry::__OpenFile(IArchiveOpenCallback &callback)
    {
        m_cpInStream = MakeSharedCom(new InFileStream(m_tstrPath, false));
    }

    void ArchiveEntry::__OpenStream(IArchiveOpenCallback &callback)
    {
        TString tstrFileName = Helper::GetFileName(m_tstrPath);
        TString tstrFileExtension = Helper::GetFileExtension(m_tstrPath);
        vector<reference_wrapper<const CodecFormat>> vCodecFormat;
        vector<reference_wrapper<const CodecFormat>> vOtherCodecFormat;
        for (const auto &format : m_cpCodecs->GetFormats())
        {
            const auto &vupExtensionInfo = format->GetExtensions();
            if (find_if(
                vupExtensionInfo.cbegin(),
                vupExtensionInfo.cend(),
                [&tstrFileExtension](const unique_ptr<ExtensionInfo> &value) {
                return Helper::ToLower(tstrFileExtension)
                    == Helper::ToLower(value->Extension);
            }) == vupExtensionInfo.cend())
            {
                // Not found.
                vOtherCodecFormat.push_back(*format);
            }
            else
            {
                vCodecFormat.push_back(*format);
            }
        }
        
        //assert(vCodecFormat.size() == 1);
        vCodecFormat.insert(
            vCodecFormat.end(),
            vOtherCodecFormat.cbegin(),
            vOtherCodecFormat.cend());

        // TODO: See also 7zFM, OpenArchive.cpp, CArc::OpenStream.
        // 1. Special order for PE format;
        // 2. Additional support for .000 and .001 files (use rar format);
        // 3. Make sure ISO format is in front of UDF format in vCodecFormat.

        for (const CodecFormat &format : vCodecFormat)
        {
            try
            {
                auto cpInArchive = format.CreateInArchive();
                m_cpInStream->Seek(0, STREAM_SEEK_SET, nullptr);

                IInArchiveAdapter<> archiveAdapter(*cpInArchive);

                auto cpSetCompressCodecsInfo
                    = archiveAdapter.QueryInterface<ISetCompressCodecsInfo>(
                    IID_ISetCompressCodecsInfo);
                if (cpSetCompressCodecsInfo)
                {
                    ISetCompressCodecsInfoAdapter<>(
                        *cpSetCompressCodecsInfo).SetCompressCodecsInfo(*m_cpCodecs);
                }

                archiveAdapter.Open(
                    m_cpInStream.get(), MAX_CHECK_START_POSITION, &callback);
                m_cpInArchive = move(cpInArchive);
                return;
            }
            catch (const SevenZipCoreException &)
            {
            }
        }

        // Error message does not seem to be used.
        //try
        //{
        //    PropertyVariant property;
        //    ErrorMessage = PropertyHelper::GetString(
        //        archiveAdapter.GetArchiveProperty(
        //        static_cast<PROPID>(PropertyId::Error)));
        //}
        //catch (const ArchiveException &)
        //{
        //}

        // Get default name of the archive.

        throw ArchiveException("Cannot open the archive.");
    }

    void ArchiveEntry::__Close()
    {
        if (m_cpInArchive)
        {
            IInArchiveAdapter<>(*m_cpInArchive).Close();
        }
    }

    void ArchiveEntry::__LoadFolder()
    {
        m_upRootFolder = make_unique<ArchiveFolder>(TEXT(""), *this);
        IInArchiveAdapter<> archiveAdapter(*m_cpInArchive);
        UINT32 un32ItemCount = archiveAdapter.GetNumberOfItems();
        TString tstrArchiveFileName = Helper::GetFileName(m_tstrPath);
        for (UINT32 i = 0; i != un32ItemCount; ++i)
        {
            auto *pCurrentFolder = m_upRootFolder.get();
            auto tstrItemPath = archiveAdapter.GetItemPath(i);
            auto vtstrFolder = Helper::SplitString(
                tstrItemPath,
                FOLDER_POSSIBLE_SEPARATOR,
                true);
            const auto tstrBack = vtstrFolder.back();
            vtstrFolder.pop_back();
            for (const auto &strFolder : vtstrFolder)
            {
                if (strFolder.empty())
                {
                    continue;
                }
                pCurrentFolder = &pCurrentFolder->AddFolder(
                    strFolder, *pCurrentFolder);
            }
            if (tstrBack.empty())
            {
                continue;
            }
            if (PropertyHelper::GetBool(archiveAdapter.GetProperty(
                i, PropertyId::IsDir), false))
            {
                pCurrentFolder->AddFolder(i, tstrBack, *pCurrentFolder);
            }
            else
            {
                pCurrentFolder->AddFile(i, tstrBack, *pCurrentFolder);
            }
        }
    }

}