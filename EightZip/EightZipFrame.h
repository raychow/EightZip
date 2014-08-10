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

    EightZipFrame(const wxString &title);
    virtual ~EightZipFrame();

private:
    void __Create();
    void __CreateMenu();
    void __CreateToolBar(wxBoxSizer *pSizerMain);
    void __CreateFileExplorer(wxBoxSizer *pSizerMain);

    void __OnExitClick(wxCommandEvent &WXUNUSED(event));

};

#endif // EIGHTZIPFRAME_H