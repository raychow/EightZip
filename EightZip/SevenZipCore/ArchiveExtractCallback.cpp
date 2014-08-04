#include "ArchiveExtractCallback.h"

#include <vector>

#include <boost/filesystem.hpp>
#include <boost/system/error_code.hpp>

#include "Common.h"

using namespace std;

namespace SevenZipCore
{
    ArchiveExtractCallback::ArchiveExtractCallback(
        std::shared_ptr<Archive> spArchive,
        bool isStandardOutMode,
        bool isTestMode,
        bool isCRCMode,
        TString tstrExtractPath,
        TString tstrInternalPath,
        PathMode pathMode,
        OverwriteMode overwriteMode)
        : m_spArchive(move(spArchive))
        , m_isStandardOutMode(isStandardOutMode)
        , m_isTestMode(isTestMode)
        , m_isCRCMode(isCRCMode)
        , m_tstrExtractPath(move(tstrExtractPath))
        , m_vtstrInternalPathPart(Helper::SplitString(move(tstrInternalPath),
            FOLDER_POSSIBLE_SEPARATOR, true))
        , m_pathMode(pathMode)
        , m_overwriteMode(overwriteMode)
    {
    }

    STDMETHODIMP ArchiveExtractCallback::SetTotal(UINT64 total)
    {
        m_un64Total = total;
        return S_OK;
    }

    STDMETHODIMP ArchiveExtractCallback::SetCompleted(
        const UINT64 *completeValue)
    {
        return S_OK;
    }

    STDMETHODIMP ArchiveExtractCallback::GetStream(
        UINT32 index, ISequentialOutStream **outStream, INT32 askExtractMode)
    {
        try
        {
            *outStream = nullptr;
            m_nindex = index;

            IInArchiveAdapter archiveAdapter(
                m_spArchive->GetArchiveEntry()->GetArchive());
            m_tstrInternalPath = archiveAdapter.GetItemPath(index);
            m_isDirectory = PropertyHelper::GetBool(
                archiveAdapter.GetProperty(index, PropertyId::IsDir), false);

            try
            {
                m_oun64Position.reset(PropertyHelper::GetUInt64(
                    archiveAdapter.GetProperty(index, PropertyId::Position)));
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
            m_isEncrypted = PropertyHelper::GetBool(archiveAdapter.GetProperty(
                index, PropertyId::Encrypted), false);
            try
            {
                m_oun64Size.reset(PropertyHelper::GetConvertedUInt64(
                    archiveAdapter.GetProperty(index, PropertyId::Size)));
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
                        archiveAdapter.GetProperty(
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
                    archiveAdapter, index, PropertyId::Created);
                m_oftAccessed = __GetTime(
                    archiveAdapter, index, PropertyId::Accessed);
                m_oftModified = __GetTime(
                    archiveAdapter, index, PropertyId::Modified);

                // TODO: What does isAnti mean?
                bool isAnti = PropertyHelper::GetBool(
                    archiveAdapter.GetProperty(index, PropertyId::IsAnti),
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
                case PathMode::FullPathNames:
                    break;
                case PathMode::CurrentPathNames:
                    if (m_vtstrInternalPathPart.size() >= vtstrPathPart.size()
                        || !equal(m_vtstrInternalPathPart.cbegin(),
                        m_vtstrInternalPathPart.cend(),
                        vtstrPathPart.cbegin()))
                    {
                        return E_FAIL;
                    }
                    vtstrPathPart.erase(
                        vtstrPathPart.begin(),
                        vtstrPathPart.begin() + m_vtstrInternalPathPart.size());
                    break;
                case PathMode::NoPathNames:
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
                        auto tstrDirectoryrPath = m_tstrExtractPath
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
                auto tstrFullPath = m_tstrExtractPath + tstrFilteredPath;
                boost::system::error_code errorCode;
                if (m_isDirectory)
                {
                    m_tstrRealPath = move(tstrFullPath);
                    if (isAnti)
                    {
                        boost::filesystem::remove(m_tstrRealPath, errorCode);
                    }
                    return S_OK;
                }

                // !_isSplit
                if (!m_oun64Position)
                {
                    if (boost::filesystem::exists(tstrFullPath))
                    {
                        switch (m_overwriteMode)
                        {
                        case OverwriteMode::SkipExisting:
                            return S_OK;
                        case OverwriteMode::AskBefore:
                        {
                            int nDialogResult =
                                static_cast<int>(OverwriteAnswer::YesToAll);
                            // nDialogResult = ShowOverwriteDialog();
                            switch (static_cast<OverwriteAnswer>(nDialogResult))
                            {
                            case OverwriteAnswer::Cancel:
                                return E_ABORT;
                            case OverwriteAnswer::No:
                                return S_OK;
                            case OverwriteAnswer::NoToAll:
                                m_overwriteMode = OverwriteMode::SkipExisting;
                                return S_OK;
                            case OverwriteAnswer::YesToAll:
                                m_overwriteMode = OverwriteMode::WithoutPrompt;
                                break;
                            case OverwriteAnswer::Yes:
                                break;
                            case OverwriteAnswer::AutoRename:
                                m_overwriteMode = OverwriteMode::AutoRename;
                                break;
                            default:
                                return E_FAIL;
                            }
                        }
                        }
                        switch (m_overwriteMode)
                        {
                        case OverwriteMode::AutoRename:
                            if (!Helper::AutoRenamePath(tstrFullPath))
                            {
                                // TODO: Add error message.
                                return E_FAIL;
                            }
                            break;
                        case OverwriteMode::AutoRenameExisting:
                            // Unused?
                            // Not implement yet.
                            return E_FAIL;
                        case OverwriteMode::WithoutPrompt:
                            boost::filesystem::remove(tstrFullPath, errorCode);
                            if (errorCode)
                            {
                                // Add error message.
                                return S_OK;
                            }
                            break;
                        default:
                            return E_FAIL;
                        }
                    }
                }
                if (!isAnti)
                {
                    // TODO: 20140804
                }
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
        return S_OK;
    }

    STDMETHODIMP ArchiveExtractCallback::SetOperationResult(
        INT32 resultEOperationResult)
    {
        return S_OK;
    }

    boost::optional<FILETIME> ArchiveExtractCallback::__GetTime(
        IInArchiveAdapter &archiveAdapter,
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