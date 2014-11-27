// ProgressDialog.h

#ifndef PROGRESSDIALOG_H
#define PROGRESSDIALOG_H

#include <atomic>
#include <chrono>
#include <functional>
#include <mutex>

#include <boost/optional.hpp>

#include "SevenZipCore/TString.h"

#include "Exception.h"

#ifdef __WXMSW__
#include "TaskbarProgressHelper.h"
#endif

class ProgressDialog
    : public wxDialog
{
public:
    enum class Mode
    {
        Open,
        Extract
    };

    class Cancelled {};

    static const int PROGRESS_MAX = 1000;
    static const int UPDATE_INTERVAL = 500;

    ProgressDialog(
        wxWindow *parent,
        wxWindowID id,
        Mode mode,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER,
        const wxString& name = wxDialogNameStr);

    virtual ~ProgressDialog();

    void SetArchivePath(const TString &tstrPath);
    void SetCurrentFile(const TString &tstrFileName);
    void SetTotal(UINT64 un64Total);
    void SetCompleted(UINT64 un64Completed);

    void SetAllTotal(UINT64 un64AllTotal);
    void SetCurrentTotal(UINT64 un64NowTotal);

    virtual bool Show(bool show = true) override;

    void UpdateDisplay();
    void Pause();
    void Resume();
    void Cancel();
    void Done(bool isSuccess);

    inline bool IsCancelled() const { return m_isCancelled; }
    void CheckCancelled() const;

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

    Mode m_mode = Mode::Extract;
    wxString m_wstrTitle;

    TString m_tstrArchiveFileName;
    TString m_tstrArchivePath;
    TString m_tstrCurrentFile;
    UINT64 m_un64Total = {};
    UINT64 m_un64Completed = {};
    UINT64 m_un64NowTotal = {};
    UINT64 m_un64Base = {};
    UINT64 m_un64NextBase = {};

    wxStaticText *m_pLabelElaspedTime = nullptr;
    wxStaticText *m_pLabelTimeLeft = nullptr;
    wxStaticText *m_pLabelArchivePath = nullptr;
    wxStaticText *m_pLabelCurrentFile = nullptr;
    wxStaticText *m_pLabelPercent = nullptr;
    wxButton *m_pButtonPause = nullptr;

    wxGauge *m_pGaugeProcessed = nullptr;

    void __Create();

    void __OnUpdate(wxTimerEvent &WXUNUSED(event));
    void __OnPauseClick(wxCommandEvent &WXUNUSED(event));
    void __OnCancelClick(wxCommandEvent &event);

};

class ProgressDialogManager
{
public:
    ProgressDialogManager(ProgressDialog *pProgressDialog)
        : m_pDialog(pProgressDialog)
    {
        wxTheApp->CallAfter(std::bind(
            // Probably be called after destruct.
            [](ProgressDialog *pDialog) {
            pDialog->ShowModal();
        }, m_pDialog));
    }

    ~ProgressDialogManager()
    {
        wxTheApp->CallAfter(std::bind(
            // Probably be called after destruct.
            [](ProgressDialog *pDialog, bool isSuccess) {
            pDialog->Done(isSuccess);
        }, m_pDialog, m_isSuccess));
    }

    void SetSuccess(bool isSuccess)
    {
        m_isSuccess = isSuccess;
    }

private:
    ProgressDialog *m_pDialog = nullptr;
    bool m_isSuccess = false;

    ProgressDialogManager(const ProgressDialogManager&) = delete;
    ProgressDialogManager &operator=(const ProgressDialogManager&) = delete;

};

#endif // PROGRESSDIALOG_H
