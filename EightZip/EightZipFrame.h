// EightZipFrame.h

#ifndef EIGHTZIPFRAME_H
#define EIGHTZIPFRAME_H

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
    bool m_isLastMaxmized = false;
    wxPoint m_lastPosition;
    wxSize m_lastSize;

    void __Create();
    void __CreateMenu();
    void __CreateToolBar(wxBoxSizer *pSizerMain);
    void __CreateFileExplorer(wxBoxSizer *pSizerMain);
    void __RevertSize();

    void __OnFileExitClick(wxCommandEvent &WXUNUSED(event));
    void __OnCommandExtractClick(wxCommandEvent &WXUNUSED(event));

    void __OnMove(wxMoveEvent& event);
    void __OnSize(wxSizeEvent& event);
    void __UpdateLastLayout();

    void __UpdateConfig();
};

#endif // EIGHTZIPFRAME_H