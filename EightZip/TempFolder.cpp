#include "stdwx.h"
#include "TempFolder.h"

#include <boost/system/error_code.hpp>

#include "Exception.h"

void TempFolder::Create()
{
    if (!m_pathFolder.empty())
    {
        Delete();
    }
    boost::system::error_code errorCode;
    auto tempPath = boost::filesystem::temp_directory_path(errorCode);
    if (!errorCode)
    {
        tempPath += boost::filesystem::unique_path("8zip-%%%%-%%%%-%%%%", errorCode);
    }
    if (!errorCode)
    {
        boost::filesystem::create_directories(tempPath, errorCode);
    }
    if (errorCode)
    {
        throw FileException("Cannot create the temp folder.");
    }
    m_pathFolder = tempPath;
}

void TempFolder::Delete()
{
    boost::system::error_code errorCode;
    if (!m_pathFolder.empty())
    {
        boost::filesystem::remove(m_pathFolder, errorCode);
        m_pathFolder.clear();
    }
    if (!m_pathFile.empty())
    {
        boost::filesystem::remove(m_pathFile, errorCode);
        m_pathFile.clear();
    }
}
