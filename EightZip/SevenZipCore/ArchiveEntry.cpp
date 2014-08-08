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
    ArchiveEntry::ArchiveEntry(
        weak_ptr<Archive> wpArchive,
        shared_ptr<Codecs> cpCodecs,
        TString tstrPath,
        shared_ptr<IInStream> cpStream,
        int nSubFileIndex,
        shared_ptr<IArchiveOpenCallback> cpCallback)
        : m_wpArchive(wpArchive)
        , m_cpCodecs(cpCodecs)
        , m_tstrPath(move(tstrPath))
        , m_cpInStream(cpStream)
        , m_nSubfileIndex(nSubFileIndex)
        , m_cpCallback(move(cpCallback))
    {
        if (!m_cpInStream)
        {
            __OpenFile();
        }
        __OpenStream();
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

    shared_ptr<ArchiveFolder> ArchiveEntry::GetRootFolder()
    {
        auto result = make_shared<ArchiveFolder>(TEXT(""), shared_from_this());
        IInArchiveAdapter archiveAdapter(m_cpInArchive);
        UINT32 un32ItemCount = archiveAdapter.GetNumberOfItems();
        TString tstrArchiveFileName = Helper::GetFileName(m_tstrPath);
        for (UINT32 i = 0; i != un32ItemCount; ++i)
        {
            shared_ptr<ArchiveFolder> spCurrentFolder = result;
            TString tstrItemPath = archiveAdapter.GetItemPath(i);
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
                spCurrentFolder = spCurrentFolder->AddFolder(
                    strFolder, spCurrentFolder);
            }
            if (tstrBack.empty())
            {
                continue;
            }
            if (PropertyHelper::GetBool(archiveAdapter.GetProperty(
                i, PropertyId::IsDir), false))
            {
                spCurrentFolder->AddFolder(i, tstrBack, spCurrentFolder);
            }
            else
            {
                spCurrentFolder->AddFile(i, tstrBack, spCurrentFolder);
            }
        }
        result->Calculate();
        return result;
    }

    void ArchiveEntry::__OpenFile()
    {
        m_cpInStream = MakeComPtr(new InFileStream(m_tstrPath, false));
    }

    void ArchiveEntry::__OpenStream()
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

                IInArchiveAdapter archiveAdapter(cpInArchive);

                auto cpSetCompressCodecsInfo
                    = archiveAdapter.QueryInterface<ISetCompressCodecsInfo>(
                    IID_ISetCompressCodecsInfo);
                if (cpSetCompressCodecsInfo)
                {
                    ISetCompressCodecsInfoAdapter(
                        cpSetCompressCodecsInfo).SetCompressCodecsInfo(*m_cpCodecs);
                }

                archiveAdapter.Open(
                    m_cpInStream.get(), MAX_CHECK_START_POSITION, m_cpCallback.get());
                m_cpInArchive = cpInArchive;
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
            IInArchiveAdapter(m_cpInArchive).Close();
        }
    }

}