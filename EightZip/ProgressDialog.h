// ProgressDialog.h

#ifndef PROGRESSDIALOG_H
#define PROGRESSDIALOG_H

#include <atomic>
#include <chrono>
#include <mutex>

#include <boost/optional.hpp>

#include "SevenZipCore/IArchive.h"
#include "SevenZipCore/TString.h"

#include "Exception.h"

#ifdef __WXMSW__
#include "TaskbarProgressHelper.h"
#endif

class ProgressDialog
    : public wxDialog
{
public:
    class Cancelled {};

    static const int PROGRESS_MAX = 1000;
    static const int UPDATE_INTERVAL = 500;

    ProgressDialog(
        wxWindow *parent,
        wxWindowID id,
        const wxString& title,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER,
        const wxString& name = wxDialogNameStr);

    virtual ~ProgressDialog();

    void SetArchivePath(const TString &tstrPath);
    void SetCurrentFile(const TString &tstrFileName);
    void SetTotal(UINT64 un64Total);
    void SetCompleted(UINT64 un64Completed);

    SevenZipCore::OverwriteAnswer AskOverwrite(
        TString tstrPath,
        boost::optional<time_t> oftExistModified,
        boost::optional<UINT64> oun64ExistSize,
        boost::optional<time_t> oftNewModified,
        boost::optional<UINT64> oun64NewSize,
        TString *ptstrNewPath);

    void Done(bool isSuccess);

    void CheckCancelled();

private:
#ifdef __WXMSW__
    Helper::TaskbarProgress m_taskerProgress;
#endif

    std::mutex m_mutex;
    std::unique_lock<std::mutex> m_ulPause;
    std::atomic<bool> m_isCancelled = false;

    wxTimer m_timer;

    std::chrono::milliseconds m_msElasped;
    std::chrono::system_clock::time_point m_tpStart;

    TString m_tstrArchiveFileName;
    TString m_tstrArchivePath;
    TString m_tstrCurrentFile;
    UINT64 m_un64Total = 0;
    UINT64 m_un64Completed = 0;

    wxStaticText *m_pLabelElaspedTime = nullptr;
    wxStaticText *m_pLabelTimeLeft = nullptr;
    wxStaticText *m_pLabelArchivePath = nullptr;
    wxStaticText *m_pLabelCurrentFile = nullptr;
    wxStaticText *m_pLabelPercent = nullptr;
    wxButton *m_pButtonPause = nullptr;

    wxGauge *m_pGaugeProcessed = nullptr;

    void __Create();
    void __StartTimer();
    void __StopTimer();

    void __DoUpdate();

    void __Update(wxTimerEvent &WXUNUSED(event));
    void __OnPauseClick(wxCommandEvent &WXUNUSED(event));
    void __OnCancelClick(wxCommandEvent &event);

};

#endif // PROGRESSDIALOG_H
