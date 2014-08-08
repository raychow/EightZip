// TempFolder.h

#ifndef TEMPFOLDER_H
#define TEMPFOLDER_H

#include <boost/filesystem.hpp>

#include "SevenZipCore/TString.h"

using namespace std;

class TempFolder
{
public:
    TempFolder(TString tstrFileName = wxEmptyString) { Create(tstrFileName); }
    ~TempFolder() { Delete(); }

    void Create(TString tstrFileName = wxEmptyString);
    void Delete();
    bool IsCreated() const { return !m_pathFolder.empty(); }

    const TString GetFileName() const
    {
        return ConvertWStringToTString(m_pathFile.filename().wstring());
    }
    const TString GetFilePath() const
    {
        return ConvertWStringToTString(m_pathFile.wstring());
    }
    void SetFilePath(const TString &value)
    {
        m_pathFile = value;
    }
    const TString GetFolderPath() const
    {
        return ConvertWStringToTString(m_pathFolder.wstring());
    }

private:
    boost::filesystem::path m_pathFile;
    boost::filesystem::path m_pathFolder;
};

#endif // TEMPFOLDER_H
