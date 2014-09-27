// ProgressDialog.h

#ifndef PROGRESSDIALOG_H
#define PROGRESSDIALOG_H

#include <mutex>

#include "SevenZipCore/TString.h"

class ProgressDialog
    : public wxDialog
{
public:
    static const int PROGRESS_MAX = 1000;

    ProgressDialog(
        wxWindow *parent,
        wxWindowID id,
        const wxString& title,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER,
        const wxString& name = wxDialogNameStr);

    ~ProgressDialog() {}

    void SetArchivePath(const TString &tstrPath);
    void SetCurrentFile(const TString &tstrFileName);
    void SetCurrentPercent(int nPercent);

private:
    std::mutex m_mutex;

    wxTimer m_timer;

    TString m_tstrArchiveFileName;
    TString m_tstrArchivePath;
    TString m_tstrCurrentFile;
    int m_nCurrentPercent = 0;

    wxStaticText *m_pLabelArchivePath = nullptr;
    wxStaticText *m_pLabelCurrentFile = nullptr;
    wxStaticText *m_pLabelCurrentPercent = nullptr;

    wxGauge *m_pGaugeCurrentFile = nullptr;
    wxGauge *m_pGaugeProcessed = nullptr;

    void __Create();
    void __StartTimer();
    void __Update(wxTimerEvent &WXUNUSED(event));
};

#endif // PROGRESSDIALOG_H
