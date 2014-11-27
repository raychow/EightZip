#include "stdwx.h"
#include "CodecsLoader.h"

#include "SevenZipCore/ComPtr.h"

using namespace std;

std::shared_ptr<SevenZipCore::Codecs> CodecsLoader::GetCodecs() const
{
    if (auto cpCodecs = m_wpCodecs.lock())
    {
        return cpCodecs;
    }
    TString tstrExecutablePath
        = wxStandardPaths::Get().GetExecutablePath();
    tstrExecutablePath = tstrExecutablePath.substr(
        0, tstrExecutablePath.find_last_of(wxFILE_SEP_PATH) + 1);
    auto result = SevenZipCore::MakeSharedCom(
        new SevenZipCore::Codecs(tstrExecutablePath));
    m_wpCodecs = result;
    return result;
}
