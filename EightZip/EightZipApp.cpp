#include "stdwx.h"
#include "EightZipApp.h"
#include "EightZipFrame.h"

IMPLEMENT_APP(EightZipApp)

bool EightZipApp::OnInit()
{
    EightZipFrame *eightZip = new EightZipFrame(wxT("Eight Zip"));
    eightZip->Show(true);

    return true;
}
