#include "ArchiveEntry.h"

#include <algorithm>
#include <cassert>
#include <vector>

#include "Common.h"
#include "Exception.h"
#include "FileStream.h"
#include "IArchiveAdapter.h"
#include "ICoderAdapter.h"

using namespace std;

namespace SevenZipCore
{
    ArchiveEntry::ArchiveEntry(
        Codecs &codecs,
        TString tstrPath,
        std::shared_ptr<IArchiveOpenCallback> cpCallback)
        : m_codecs(codecs)
        , m_tstrPath(move(tstrPath))
        , m_cpCallback(move(cpCallback))
    {
        __OpenFile();
    }

    ArchiveEntry::ArchiveEntry(
        Codecs &codecs,
        TString tstrPath,
        shared_ptr<IInStream> cpStream,
        int nSubFileIndex, std::shared_ptr<IArchiveOpenCallback> cpCallback)
        : m_codecs(codecs)
        , m_tstrPath(move(tstrPath))
        , m_cpInStream(cpStream)
        , m_nSubfileIndex(nSubFileIndex)
        , m_cpCallback(move(cpCallback))
    {
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

    void ArchiveEntry::__OpenFile()
    {
        // if stream
        //   open stream
        // else
        m_cpInStream = MakeComPtr(new InFileStream(m_tstrPath));
        __OpenStream();
    }

    void ArchiveEntry::__OpenStream()
    {
        TString tstrFileName = Helper::GetFileName(m_tstrPath);
        TString tstrFileExtension = Helper::GetFileExtension(m_tstrPath);
        vector<reference_wrapper<const CodecFormat>> vCodecFormat;
        vector<reference_wrapper<const CodecFormat>> vOtherCodecFormat;
        for (const auto &format : m_codecs.GetFormats())
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
            m_cpArchive = format.CreateInArchive();
            if (m_cpArchive)
            {
                break;
            }
        }
        if (!m_cpArchive)
        {
            throw ArchiveException("Cannot open the archive.");
        }
        m_cpInStream->Seek(0, STREAM_SEEK_SET, nullptr);

        IInArchiveAdapter archiveAdapter(m_cpArchive);

        auto cpSetCompressCodecsInfo
            = archiveAdapter.QueryInterface<ISetCompressCodecsInfo>(
            IID_ISetCompressCodecsInfo);
        if (cpSetCompressCodecsInfo)
        {
            ISetCompressCodecsInfoAdapter(
                cpSetCompressCodecsInfo).SetCompressCodecsInfo(m_codecs);
        }

        archiveAdapter.Open(
            m_cpInStream.get(), MAX_CHECK_START_POSITION, m_cpCallback.get());


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
    }

    void ArchiveEntry::__Close()
    {
        if (m_cpArchive)
        {
            IInArchiveAdapter(m_cpArchive).Close();
        }
    }

}