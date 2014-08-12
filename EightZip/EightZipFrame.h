// EightZipFrame.h

#ifndef EIGHTZIPFRAME_H
#define EIGHTZIPFRAME_H

#include "WindowStateManager.h"

class FileExplorer;

class EightZipFrame
    : public wxFrame
{
public:
    enum
    {
        ID_FILE_OPEN = wxID_HIGHEST + 1,
        ID_FILE_EXIT,
        ID_COMMAND_ADD,
        ID_COMMAND_EXTRACT,
        ID_PARENT_FOLDER
    };

    EightZipFrame();

    virtual ~EightZipFrame();

private:
    WindowStateManager m_windowStateManager;

    FileExplorer *m_pFileExplorer = nullptr;

    void __Create();
    void __CreateMenu();
    void __CreateToolBar(wxBoxSizer *pSizerMain);
    void __CreateFileExplorer(wxBoxSizer *pSizerMain);

    void __OnFileExitClick(wxCommandEvent &WXUNUSED(event));
    void __OnCommandExtractClick(wxCommandEvent &WXUNUSED(event));

};

#endif // EIGHTZIPFRAME_H