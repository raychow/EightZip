#include "stdwx.h"
#include "EightZipApp.h"

#include <wx/imagpng.h>

#include "EightZipFrame.h"

IMPLEMENT_APP(EightZipApp)

bool EightZipApp::OnInit()
{
    wxImage::AddHandler(new wxPNGHandler);
    EightZipFrame *eightZip = new EightZipFrame();
    SetTopWindow(eightZip);
    eightZip->Show(true);

    return true;
}

int EightZipApp::OnExit()
{
    EightZipConfig::DestroyInstance();
    return 0;
}
