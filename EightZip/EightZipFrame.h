// EightZipFrame.h

#ifndef EIGHTZIPFRAME_H
#define EIGHTZIPFRAME_H

class EightZipFrame
    : public wxFrame
{
public:
    enum
    {
        ID_OPEN = wxID_HIGHEST + 1,
        ID_EXIT
    };

    EightZipFrame(const wxString &title);
    virtual ~EightZipFrame();

private:
    void __CreateMenu();
    void __CreateToolBar();

    void __OnExitClick(wxCommandEvent &WXUNUSED(event));
    void __Create();
    wxAuiManager m_auiManager;
};

#endif // EIGHTZIPFRAME_H