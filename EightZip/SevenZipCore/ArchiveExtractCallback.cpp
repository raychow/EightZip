#include "ArchiveExtractCallback.h"

#include <vector>

#include <boost/filesystem.hpp>
#include <boost/system/error_code.hpp>

#include "ArchiveEntry.h"
#include "CommonHelper.h"
#include "ComPtr.h"
#include "Exception.h"
#include "IArchiveAdapter.h"

using namespace std;

namespace SevenZipCore
{
    ArchiveExtractCallback::ArchiveExtractCallback(
        std::shared_ptr<Archive> spArchive,
        bool isStandardOutMode,
        bool isTestMode,
        bool isCRCMode,
        TString tstrExtractFolder,
        TString tstrCurrentBeginPath,
        ExtractPathMode pathMode,
        ExtractOverwriteMode overwriteMode)
        : m_spArchive(move(spArchive))
        , m_isStandardOutMode(isStandardOutMode)
        , m_isTestMode(isTestMode)
        , m_isCRCMode(isCRCMode)
        , m_tstrExtractFolder(Helper::MakePathSlash(move(tstrExtractFolder)))
        , m_vtstrCurrentBeginPathPart(Helper::SplitString(
        move(tstrCurrentBeginPath), FOLDER_POSSIBLE_SEPARATOR, true))
        , m_pathMode(pathMode)
        , m_overwriteMode(overwriteMode)
    {
    }

    STDMETHODIMP ArchiveExtractCallback::SetTotal(UINT64 total)
    {
        m_un64Total = total;
        // TODO: Set the progress window.
        return S_OK;
    }

    STDMETHODIMP ArchiveExtractCallback::SetCompleted(
        const UINT64 *completeValue)
    {
        // TODO: Set the progress window.
        return S_OK;
    }

    STDMETHODIMP ArchiveExtractCallback::GetStream(
        UINT32 index, ISequentialOutStream **outStream, INT32 askExtractMode)
    {
        try
        {
            *outStream = nullptr;
            m_nindex = index;

            IInArchiveAdapter<> inArchiveAdapter(
                m_spArchive->GetArchiveEntry()->GetInArchive());
            m_tstrInternalPath = inArchiveAdapter.GetItemPath(index);
            m_isDirectory = PropertyHelper::GetBool(
                inArchiveAdapter.GetProperty(index, PropertyId::IsDir), false);

            try
            {
                m_oun64Position.reset(PropertyHelper::GetUInt64(
                    inArchiveAdapter.GetProperty(index, PropertyId::Position)));
            }
            catch (const PropertyException &ex)
            {
                if (PropertyErrorCode::EMPTY_VALUE == ex.GetErrorCode())
                {
                    m_oun64Position.reset();
                }
                else
                {
                    throw;
                }
            }
            m_isEncrypted = PropertyHelper::GetBool(inArchiveAdapter.GetProperty(
                index, PropertyId::Encrypted), false);

            if (static_cast<INT32>(ExtractAskMode::Extract) == askExtractMode
                && !m_isTestMode)
            {
                if (m_isStandardOutMode)
                {
                    // CMyComPtr<ISequentialOutStream> outStreamLoc
                    //     = new CStdOutFileStream;
                    return S_OK;
                }
                try
                {
                    m_ounAttribute.reset(PropertyHelper::GetUInt32(
                        inArchiveAdapter.GetProperty(
                        index, PropertyId::Attribute))
                        );
                }
                catch (const PropertyException &ex)
                {
                    if (PropertyErrorCode::EMPTY_VALUE == ex.GetErrorCode())
                    {
                        m_ounAttribute.reset();
                    }
                    else
                    {
                        throw;
                    }
                }

                m_oftCreated = __GetTime(
                    inArchiveAdapter, index, PropertyId::Created);
                m_oftAccessed = __GetTime(
                    inArchiveAdapter, index, PropertyId::Accessed);
                m_oftModified = __GetTime(
                    inArchiveAdapter, index, PropertyId::Modified);

                // TODO: What does isAnti mean?
                bool isAnti = PropertyHelper::GetBool(
                    inArchiveAdapter.GetProperty(index, PropertyId::IsAnti),
                    false);
                auto vtstrPathPart = Helper::SplitString(
                    m_tstrInternalPath,
                    FOLDER_POSSIBLE_SEPARATOR,
                    true);
                if (vtstrPathPart.empty())
                {
                    return E_FAIL;
                }
                switch (m_pathMode)
                {
                case ExtractPathMode::FullPathNames:
                    break;
                case ExtractPathMode::CurrentPathNames:
                    if (m_vtstrCurrentBeginPathPart.size() >= vtstrPathPart.size()
                        || !equal(m_vtstrCurrentBeginPathPart.cbegin(),
                        m_vtstrCurrentBeginPathPart.cend(),
                        vtstrPathPart.cbegin()))
                    {
                        return E_FAIL;
                    }
                    vtstrPathPart.erase(
                        vtstrPathPart.begin(),
                        vtstrPathPart.begin() + m_vtstrCurrentBeginPathPart.size());
                    break;
                case ExtractPathMode::NoPathNames:
                    vtstrPathPart = { vtstrPathPart.back() };
                }
                vtstrPathPart = Helper::GetFilteredPath(move(vtstrPathPart));
                auto tstrFilteredPath = Helper::JoinString(
                    vtstrPathPart,
                    FOLDER_SEPARATOR_STRING);
                if (!isAnti)
                {
                    if (!m_isDirectory)
                    {
                        if (!vtstrPathPart.empty())
                        {
                            vtstrPathPart.pop_back();
                        }
                    }
                    //  Used for extracting the folder with sub directories.
                    if (!vtstrPathPart.empty())
                    {
                        auto tstrDirectoryrPath = m_tstrExtractFolder
                            + Helper::JoinString(vtstrPathPart,
                            FOLDER_SEPARATOR_STRING);
                        boost::filesystem::create_directories(
                            tstrDirectoryrPath);
                        try
                        {
                            Helper::SetFileTime(
                                tstrDirectoryrPath,
                                m_oftCreated ? &*m_oftCreated : nullptr,
                                m_oftAccessed ? &*m_oftAccessed : nullptr,
                                m_oftModified ? &*m_oftModified :
                                m_spArchive->GetModifiedTime() ?
                                &*m_spArchive->GetModifiedTime() : nullptr);
                        }
                        catch (const SystemException &)
                        {
                        }
                    }
                }
                auto tstrFullPath = m_tstrExtractFolder + tstrFilteredPath;
                boost::system::error_code errorCode;
                if (m_isDirectory)
                {
                    m_tstrExtractPath = move(tstrFullPath);
                    if (isAnti)
                    {
                        boost::filesystem::remove(m_tstrExtractPath, errorCode);
                    }
                    return S_OK;
                }

                // !_isSplit
                if (!m_oun64Position && boost::filesystem::exists(tstrFullPath))
                {
                    switch (m_overwriteMode)
                    {
                    case ExtractOverwriteMode::SkipExisting:
                        return S_OK;
                    case ExtractOverwriteMode::AskBefore:
                    {
                        boost::optional<UINT64> oun64Size;
                        try
                        {
                            oun64Size.reset(PropertyHelper::GetConvertedUInt64(
                                inArchiveAdapter.GetProperty(
                                m_nindex, PropertyId::Size)));
                        }
                        catch (const PropertyException &)
                        {
                        }
                        int nDialogResult =
                            static_cast<int>(OverwriteAnswer::YesToAll);
                        // TODO: nDialogResult = ShowOverwriteDialog();
                        switch (static_cast<OverwriteAnswer>(nDialogResult))
                        {
                        case OverwriteAnswer::Cancel:
                            return E_ABORT;
                        case OverwriteAnswer::No:
                            return S_OK;
                        case OverwriteAnswer::NoToAll:
                            m_overwriteMode
                                = ExtractOverwriteMode::SkipExisting;
                            return S_OK;
                        case OverwriteAnswer::YesToAll:
                            m_overwriteMode
                                = ExtractOverwriteMode::WithoutPrompt;
                            break;
                        case OverwriteAnswer::Yes:
                            break;
                        case OverwriteAnswer::AutoRename:
                            m_overwriteMode = ExtractOverwriteMode::AutoRename;
                            break;
                        default:
                            return E_FAIL;
                        }
                    }
                    }
                    switch (m_overwriteMode)
                    {
                    case ExtractOverwriteMode::AutoRename:
                        if (!Helper::AutoRenamePath(tstrFullPath))
                        {
                            // TODO: Add error message.
                            return E_FAIL;
                        }
                        break;
                    case ExtractOverwriteMode::AutoRenameExisting:
                        // Unused?
                        // Not implement yet.
                        return E_FAIL;
                    case ExtractOverwriteMode::WithoutPrompt:
                        boost::filesystem::remove(tstrFullPath, errorCode);
                        if (errorCode)
                        {
                            // TODO: Add error message.
                            return S_OK;
                        }
                        break;
                    default:
                        return E_FAIL;
                    }
                }
                if (!isAnti)
                {
                    try
                    {
                        m_cpOutStream = MakeComPtr(
                            new OutFileStream(tstrFullPath, !m_oun64Position));
                    }
                    catch (const FileException &)
                    {
                        // TODO: Add error message.
                        return S_OK;
                    }
                    OutFileStreamAdapter streamAdapter(m_cpOutStream);
                    if (m_oun64Position)
                    {
                        streamAdapter.Seek(*m_oun64Position, STREAM_SEEK_SET);
                    }
                    // The archive handler does not add ref for out stream,
                    // but still release it.
                    m_cpOutStream->AddRef();
                    *outStream = m_cpOutStream.get();
                }
                m_tstrExtractPath = tstrFullPath;
            }
            else
            {
                *outStream = nullptr;
            }
            if (m_isCRCMode)
            {
                // TODO: CRCMode
            }
            return S_OK;
        }
        catch (...)
        {
            return E_FAIL;
        }
    }

    STDMETHODIMP ArchiveExtractCallback::PrepareOperation(INT32 askExtractMode)
    {
        m_isExtractMode = false;
        if (static_cast<int>(ExtractAskMode::Extract) == askExtractMode)
        {
            if (m_isTestMode)
            {
                askExtractMode = static_cast<int>(ExtractAskMode::Test);
            }
            else
            {
                m_isExtractMode = true;
            }
        }
        // TODO: Prepare for progress window.
        return S_OK;
    }

    STDMETHODIMP ArchiveExtractCallback::SetOperationResult(
        INT32 resultEOperationResult)
    {
        try
        {
            switch (static_cast<ExtractResult>(resultEOperationResult))
            {
            case ExtractResult::OK:
            case ExtractResult::UnSupportedMethod:
            case ExtractResult::CRCError:
            case ExtractResult::DataError:
                break;
            default:
                m_cpOutStream.reset();
                return E_FAIL;
            }
            // if m_cpCRCStream
            if (m_cpOutStream)
            {
                OutFileStreamAdapter streamAdapter(m_cpOutStream);
                streamAdapter.SetTime(
                    m_oftCreated ? &*m_oftCreated : nullptr,
                    m_oftAccessed ? &*m_oftAccessed : nullptr,
                    m_oftModified ? &*m_oftModified : nullptr);
                //m_oun64Size = streamAdapter.GetProcessedSize();
                streamAdapter.Close();
                m_cpOutStream.reset();
            }

            return S_OK;
        }
        catch (...)
        {
            return E_FAIL;
        }
    }

    boost::optional<FILETIME> ArchiveExtractCallback::__GetTime(
        IInArchiveAdapter<> &archiveAdapter,
        UINT32 index,
        PropertyId propertyId)
    {
        try
        {
            return PropertyHelper::GetFileTime(archiveAdapter.GetProperty(
                index, propertyId));
        }
        catch (const PropertyException &ex)
        {
            if (PropertyErrorCode::EMPTY_VALUE == ex.GetErrorCode())
            {
                return boost::none;
            }
            else
            {
                throw;
            }
        }
    }

}