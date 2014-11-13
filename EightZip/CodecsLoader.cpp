#include "stdwx.h"
#include "CodecsLoader.h"

#include "SevenZipCore/ComPtr.h"

using namespace std;

CodecsLoader::CodecsLoader()
{
    TString tstrExecutablePath
        = wxStandardPaths::Get().GetExecutablePath();
    tstrExecutablePath = tstrExecutablePath.substr(
        0, tstrExecutablePath.find_last_of(wxFILE_SEP_PATH) + 1);
    m_cpCodecs = SevenZipCore::MakeSharedCom(
        new SevenZipCore::Codecs(tstrExecutablePath));

}
