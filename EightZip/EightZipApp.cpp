#include "stdwx.h"
#include "EightZipApp.h"

#include <wx/imagpng.h>

#include "EightZipFrame.h"

IMPLEMENT_APP(EightZipApp)

bool EightZipApp::OnInit()
{
    wxImage::AddHandler(new wxPNGHandler);
    EightZipFrame *eightZip = new EightZipFrame(wxT("Eight Zip"));
    eightZip->Show(true);

    return true;
}
