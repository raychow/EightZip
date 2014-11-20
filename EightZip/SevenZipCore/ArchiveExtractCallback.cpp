#include "ArchiveExtractCallback.h"

#include <vector>

#include <boost/filesystem.hpp>
#include <boost/system/error_code.hpp>

#include "ArchiveEntry.h"
#include "CommonHelper.h"
#include "ComPtr.h"
#include "Exception.h"
#include "IArchiveAdapter.h"
#include "IExtractIndicator.h"

using namespace std;

namespace SevenZipCore
{
    ArchiveExtractCallback::ArchiveExtractCallback(Archive &archive,
        bool isStandardOutMode,
        bool isTestMode,
        bool isCRCMode,
        TString tstrExtractFolder,
        TString tstrCurrentBeginPath,
        ExtractPathMode pathMode,
        ExtractOverwriteMode overwriteMode,
        IExtractIndicator *pExtractIndicator)
        : m_archive(archive)
        , m_isStandardOutMode(isStandardOutMode)
        , m_isTestMode(isTestMode)
        , m_isCRCMode(isCRCMode)
        , m_tstrExtractFolder(Helper::MakePathSlash(move(tstrExtractFolder)))
        , m_vtstrCurrentBeginPathPart(Helper::SplitString(
        move(tstrCurrentBeginPath), FOLDER_POSSIBLE_SEPARATOR, true))
        , m_pathMode(pathMode)
        , m_overwriteMode(overwriteMode)
        , m_pExtractIndicator(pExtractIndicator)
    {
    }

    STDMETHODIMP ArchiveExtractCallback::SetTotal(UINT64 total)
    {
        try
        {
            m_un64Total = total;
            if (m_pExtractIndicator)
            {
                m_pExtractIndicator->SetTotal(total);
            }
            return S_OK;
        }
        catch (...)
        {
            return E_FAIL;
        }
    }

    STDMETHODIMP ArchiveExtractCallback::SetCompleted(
        const UINT64 *completeValue)
    {
        try
        {
            if (m_pExtractIndicator)
            {
                m_pExtractIndicator->SetCompleted(completeValue ?
                    boost::optional<UINT64>(*completeValue) : boost::none);
            }
            return S_OK;
        }
        catch (...)
        {
            return E_FAIL;
        }
    }

    STDMETHODIMP ArchiveExtractCallback::GetStream(
        UINT32 index, ISequentialOutStream **outStream, INT32 askExtractMode)
    {
        try
        {
            *outStream = nullptr;
            m_nindex = index;

            IInArchiveAdapter<> inArchiveAdapter(
                m_archive.GetArchiveEntry().GetInArchive());
            m_tstrInternalPath = inArchiveAdapter.GetItemPath(index);

            try
            {
                m_oun64Size = PropertyHelper::GetUInt64(
                    inArchiveAdapter.GetProperty(index, PropertyId::Size));
            }
            catch (const PropertyException &ex)
            {
                if (PropertyErrorCode::EMPTY_VALUE == ex.GetErrorCode())
                {
                    m_oun64Size.reset();
                }
                else
                {
                    throw;
                }
            }

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
                                m_archive.GetModifiedTime() ?
                                &*m_archive.GetModifiedTime() : nullptr);
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

                        TString tstrNewPath;
                        switch (m_pExtractIndicator->AskOverwrite(tstrFullPath,
                            boost::filesystem::last_write_time(tstrFullPath),
                            boost::filesystem::file_size(tstrFullPath),
                            m_oftModified ? boost::optional<time_t>(
                            Helper::GetUnixTimeFromFileTime(
                            *m_oftModified)) : boost::none,
                            m_oun64Size,
                            &tstrNewPath
                            ))
                        {
                        case OverwriteAnswer::Yes:
                            break;
                        case OverwriteAnswer::No:
                            return S_OK;
                        case OverwriteAnswer::Rename:
                            tstrFullPath = tstrNewPath;
                            break;
                        case OverwriteAnswer::YesToAll:
                            m_overwriteMode
                                = ExtractOverwriteMode::WithoutPrompt;
                            break;
                        case OverwriteAnswer::NoToAll:
                            m_overwriteMode
                                = ExtractOverwriteMode::SkipExisting;
                            return S_OK;
                        case OverwriteAnswer::AutoRename:
                            m_overwriteMode = ExtractOverwriteMode::AutoRename;
                            break;
                        case OverwriteAnswer::Cancel:
                            return E_ABORT;
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
                            if (m_pExtractIndicator)
                            {
                                m_pExtractIndicator->AddError(TEXT(
                                    "Error: Cannot create file \"%s\" with auto name."),
                                    tstrFullPath);
                            }
                            return E_FAIL;
                        }
                        break;
                    case ExtractOverwriteMode::AutoRenameExisting:
                        // Unused?
                        // Not implement yet.
                        return E_FAIL;
                    default:
                        boost::filesystem::remove(tstrFullPath, errorCode);
                        if (errorCode)
                        {
                            if (m_pExtractIndicator)
                            {
                                m_pExtractIndicator->AddError(TEXT(
                                    "Error: Cannot delete output file \"%s\"."),
                                    tstrFullPath);
                            }
                            return S_OK;
                        }
                    }
                }
                if (!isAnti)
                {
                    try
                    {
                        m_cpOutStream = MakeSharedCom(
                            new OutFileStream(tstrFullPath, !m_oun64Position));
                    }
                    catch (const FileException &)
                    {
                        if (m_pExtractIndicator)
                        {
                            m_pExtractIndicator->AddError(
                                TEXT("Cannot open output file \"%s\"."),
                                tstrFullPath);
                        }
                        return S_OK;
                    }
                    OutFileStreamAdapter<> streamAdapter(*m_cpOutStream);
                    if (m_oun64Position)
                    {
                        streamAdapter.Seek(*m_oun64Position, STREAM_SEEK_SET);
                    }
                    // The archive handler does not add ref for out stream,
                    // but still release it.
                    m_cpOutStream->AddRef();
                    *outStream = m_cpOutStream.get();
                }
                m_tstrExtractPath = move(tstrFullPath);
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
        try
        {
            m_isExtractMode = false;
            auto extractAskMode = static_cast<ExtractAskMode>(askExtractMode);
            if (ExtractAskMode::Extract == extractAskMode)
            {
                if (m_isTestMode)
                {
                    extractAskMode = ExtractAskMode::Test;
                }
                else
                {
                    m_isExtractMode = true;
                }
            }
            if (m_pExtractIndicator)
            {
                m_pExtractIndicator->Prepare(m_tstrInternalPath,
                    m_isDirectory, extractAskMode, m_oun64Position);
            }
            return S_OK;
        }
        catch (...)
        {
            return E_FAIL;
        }
    }

    STDMETHODIMP ArchiveExtractCallback::SetOperationResult(
        INT32 operationResult)
    {
        try
        {
            auto extractResult = static_cast<ExtractResult>(operationResult);
            switch (extractResult)
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
                OutFileStreamAdapter<> streamAdapter(*m_cpOutStream);
                streamAdapter.SetTime(
                    m_oftCreated ? &*m_oftCreated : nullptr,
                    m_oftAccessed ? &*m_oftAccessed : nullptr,
                    m_oftModified ? &*m_oftModified : nullptr);
                m_oun64Size = streamAdapter.GetProcessedSize();
                streamAdapter.Close();
                m_cpOutStream.reset();
            }
            if (m_pExtractIndicator)
            {
                m_pExtractIndicator->SetOperationResult(extractResult);
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
