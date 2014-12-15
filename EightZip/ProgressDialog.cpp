#include "stdwx.h"
#include "ProgressDialog.h"

#include <thread>

#include "SevenZipCore/CommonHelper.h"

#include "ScopeGuard.h"

using namespace std;

ProgressDialog::ProgressDialog(
    wxWindow *parent,
    wxWindowID id,
    Mode mode,
    const wxPoint &pos /*= wxDefaultPosition*/,
    const wxSize &size /*= wxDefaultSize*/,
    long style /*= wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER*/,
    const wxString &name /*= wxDialogNameStr*/)
    : wxDialog(parent, id, wxEmptyString, pos, size, style, name)
#ifdef __WXMSW__
    , m_taskerProgress(wxTheApp->GetTopWindow()->GetHandle())
#endif
    , m_ulPause(m_mutexUpdate, defer_lock)
    , m_mode(mode)
{
    switch (mode)
    {
    case Mode::Open:
        m_wstrTitle = _("Opening from %s");
        break;
    case Mode::Extract:
        m_wstrTitle = _("Extracting from %s");
        break;
    }

    __Create();
}

ProgressDialog::~ProgressDialog()
{
}

void ProgressDialog::SetArchivePath(const TString &tstrPath)
{
    lock_guard<mutex> lg(m_mutexUpdate);
    CheckCancelled();
    m_tstrArchiveFileName = SevenZipCore::Helper::GetFileName(tstrPath);
    m_tstrArchivePath = tstrPath;
}

void ProgressDialog::SetCurrentFile(const TString &tstrFileName)
{
    lock_guard<mutex> lg(m_mutexUpdate);
    CheckCancelled();
    m_tstrCurrentFile = tstrFileName;
}

void ProgressDialog::SetTotal(UINT64 un64Total)
{
    lock_guard<mutex> lg(m_mutexUpdate);
    CheckCancelled();
    m_un64Base = m_un64NextBase;
    m_un64NextBase += un64Total;
    m_un64Completed = m_un64Base;

    m_un64Total = m_un64Total + un64Total - m_un64NowTotal;
}

void ProgressDialog::SetCompleted(UINT64 un64Completed)
{
    lock_guard<mutex> lg(m_mutexUpdate);
    CheckCancelled();
    m_un64Completed = m_un64Base + un64Completed;
#ifdef __WXMSW__
    m_taskerProgress.SetState(TBPF_NORMAL);
    m_taskerProgress.SetValue(m_un64Completed, m_un64Total);
#endif
}

void ProgressDialog::SetAllTotal(UINT64 un64AllTotal)
{
    m_un64Total = un64AllTotal;
}

void ProgressDialog::SetCurrentTotal(UINT64 un64NowTotal)
{
    m_un64NowTotal = un64NowTotal;
}

bool ProgressDialog::Show(bool show /*= true*/)
{
    if (show)
    {
        Resume();
        UpdateDisplay();
    }
    else
    {
        Pause();
    }
    return wxDialog::Show(show);
}

void ProgressDialog::ShowModalDelayed(int delay /*= 1000*/)
{
    if (IsShown())
    {
        return;
    }
    wxTimer timer { this, ID_SHOW_TIMER };
    timer.StartOnce(delay);
    timer.Notify();
    ShowModalDetector detector { *this };
    {
        auto *pTopWindow = wxTheApp->GetTopWindow();
        pTopWindow->Enable(false);
        ON_SCOPE_EXIT([&] { pTopWindow->Enable(true); });
        while (!detector.IsShow() && !m_isClosed)
        {
            wxTheApp->Dispatch();
        }
    }
    detector.RemoveFilter();
    if (detector.IsShow())
    {
        ShowModal();
    }
}

void ProgressDialog::CancelDelay()
{
    wxShowEvent event(GetId(), true);
    event.SetEventObject(this);

    HandleWindowEvent(event);
}

void ProgressDialog::UpdateDisplay()
{
    lock_guard<mutex> lg(m_mutexUpdate);
    int nElasped = (m_msElasped + chrono::duration_cast<chrono::milliseconds>(
        chrono::system_clock::now() - m_tpStart)).count();

    int nElaspedSecond = nElasped / 1000;
    int nElaspedMinute = nElaspedSecond / 60;
    int nElaspedHour = nElaspedMinute / 60;
    m_pLabelElaspedTime->SetLabel(wxString::Format("%02d:%02d:%02d",
        nElaspedHour, nElaspedMinute % 60, nElaspedSecond % 60));

    if (0 != m_un64Completed)
    {
        int nLeft = nElasped * (m_un64Total - m_un64Completed) / m_un64Completed;
        int nLeftSecond = nLeft / 1000;
        int nLeftMinute = nLeftSecond / 60;
        int nLeftHour = nLeftMinute / 60;
        m_pLabelTimeLeft->SetLabel(wxString::Format(
            "%02d:%02d:%02d", nLeftHour, nLeftMinute % 60, nLeftSecond % 60));
    }

    SetTitle(wxString::Format(m_wstrTitle,
        m_tstrArchiveFileName));
    m_pLabelArchivePath->SetLabel(wxString::Format(_("Archive %s"),
        m_tstrArchivePath));
    m_pLabelCurrentFile->SetLabel(m_tstrCurrentFile);
    int nPercent = 0 == m_un64Total ? 0
        : static_cast<double>(m_un64Completed) / m_un64Total * PROGRESS_MAX;
    m_pLabelPercent->SetLabel(
        wxString::Format(_("%d%%"), nPercent / (PROGRESS_MAX / 100)));
    m_pGaugeProcessed->SetValue(nPercent);
}

void ProgressDialog::Pause()
{
    if (m_timerUpdate.IsRunning())
    {
        m_timerUpdate.Stop();
        m_msElasped += chrono::duration_cast<chrono::milliseconds>(
            chrono::system_clock::now() - m_tpStart);
        m_tpStart = chrono::system_clock::now();
    }
}

void ProgressDialog::Resume()
{
    if (!m_timerUpdate.IsRunning())
    {
        m_tpStart = chrono::system_clock::now();
        m_timerUpdate.Start(UPDATE_INTERVAL);
    }
}

void ProgressDialog::Cancel()
{
    m_isCancelled = true;
    Pause();
    if (m_ulPause.owns_lock())
    {
        m_ulPause.unlock();
    }
}

void ProgressDialog::Done(bool isSuccess)
{
    m_isClosed = true;
#ifdef __WXMSW__
    if (isSuccess)
    {
        FlashWindow(wxTheApp->GetTopWindow()->GetHandle(), true);
    }
#endif
    Pause();
    Hide();
}

void ProgressDialog::CheckCancelled() const
{
    if (m_isCancelled)
    {
        throw Cancelled();
    }
}

void ProgressDialog::__Create()
{
    auto *pSizerMain = new wxBoxSizer(wxVERTICAL);

    auto *pSizerStatus = new wxStaticBoxSizer(
        new wxStaticBox(this, wxID_ANY, wxEmptyString), wxVERTICAL);

    auto *pSizerElapsedTime = new wxBoxSizer(wxHORIZONTAL);
    pSizerElapsedTime->Add(new wxStaticText(this, wxID_ANY, _("Elapsed time")));
    m_pLabelElaspedTime = new wxStaticText(this, wxID_ANY, "00:00:00");
    m_pLabelElaspedTime->SetWindowStyle(wxALIGN_RIGHT | wxST_NO_AUTORESIZE);
    pSizerElapsedTime->Add(m_pLabelElaspedTime, wxSizerFlags().Proportion(1));
    pSizerStatus->Add(pSizerElapsedTime, wxSizerFlags().Expand());

    auto *pSizerTimeLeft = new wxBoxSizer(wxHORIZONTAL);
    pSizerTimeLeft->Add(new wxStaticText(this, wxID_ANY, _("Time left")));
    m_pLabelTimeLeft = new wxStaticText(this, wxID_ANY, "00:00:00");
    m_pLabelTimeLeft->SetWindowStyle(wxALIGN_RIGHT | wxST_NO_AUTORESIZE);
    pSizerTimeLeft->Add(m_pLabelTimeLeft, wxSizerFlags().Proportion(1));
    pSizerStatus->Add(pSizerTimeLeft, wxSizerFlags().Expand());

    pSizerStatus->Add(new wxStaticLine(this),
        wxSizerFlags().Expand().Border(wxTOP | wxBOTTOM, 5));

    m_pLabelArchivePath = new wxStaticText(this, wxID_ANY, _("Archive %s"));
    m_pLabelArchivePath->SetWindowStyle(
        wxST_NO_AUTORESIZE | wxST_ELLIPSIZE_END);
    pSizerStatus->Add(m_pLabelArchivePath, wxSizerFlags().Expand());

    m_pLabelCurrentFile = new wxStaticText(this, wxID_ANY, "%s");
    m_pLabelCurrentFile->SetWindowStyle(
        wxST_NO_AUTORESIZE | wxST_ELLIPSIZE_END);
    pSizerStatus->Add(m_pLabelCurrentFile, wxSizerFlags().Expand());

    auto *pSizerProcessed = new wxBoxSizer(wxHORIZONTAL);
    pSizerProcessed->Add(new wxStaticText(this, wxID_ANY, _("Processed")));
    m_pLabelPercent = new wxStaticText(this, wxID_ANY, "100%");
    m_pLabelPercent->SetWindowStyle(wxALIGN_RIGHT | wxST_NO_AUTORESIZE);
    pSizerProcessed->Add(m_pLabelPercent, wxSizerFlags().Proportion(1));
    pSizerStatus->Add(pSizerProcessed, wxSizerFlags().Expand());

    m_pGaugeProcessed = new wxGauge(this, wxID_ANY, PROGRESS_MAX);
    pSizerStatus->Add(m_pGaugeProcessed,
        wxSizerFlags().Expand());

    pSizerMain->AddSpacer(3);
    pSizerMain->Add(pSizerStatus,
        wxSizerFlags().Expand().Border(wxLEFT | wxRIGHT, 8));

    pSizerMain->AddStretchSpacer();

    auto *pSizerButton = new wxBoxSizer(wxHORIZONTAL);
    //pSizerButton->Add(new wxButton(this, wxID_ANY, _("&Background")),
    //    wxSizerFlags().Border(wxRIGHT, 5));
    m_pButtonPause = new wxButton(this, wxID_ANY, _("&Pause"));
    pSizerButton->Add(m_pButtonPause, wxSizerFlags().Border(wxRIGHT, 5));
    auto *pButtonCancel = new wxButton(this, wxID_CANCEL, _("Cancel"));
    pSizerButton->Add(pButtonCancel,
        wxSizerFlags());
    pSizerMain->Add(pSizerButton,
        wxSizerFlags().Border(wxALL, 7).Right());

    SetSizer(pSizerMain);
    SetSize(350, 220);
    CenterOnParent();

    m_pButtonPause->Bind(wxEVT_BUTTON, &ProgressDialog::__OnPauseClick, this);
    pButtonCancel->Bind(wxEVT_BUTTON, &ProgressDialog::__OnCancelClick, this);

    m_timerUpdate.Bind(wxEVT_TIMER, &ProgressDialog::__OnUpdate, this);
}

void ProgressDialog::__OnUpdate(wxTimerEvent &WXUNUSED(event))
{
    UpdateDisplay();
}

void ProgressDialog::__OnPauseClick(wxCommandEvent &WXUNUSED(event))
{
    if (m_ulPause.owns_lock())
    {
        m_pButtonPause->SetLabel(_("&Pause"));
        m_ulPause.unlock();
#ifdef __WXMSW__
        m_taskerProgress.SetState(TBPF_NORMAL);
#endif
        Resume();
    }
    else
    {
        m_pButtonPause->SetLabel(_("&Continue"));
        Pause();
#ifdef __WXMSW__
        m_taskerProgress.SetState(TBPF_PAUSED);
#endif
        m_ulPause.lock();
    }
}

void ProgressDialog::__OnCancelClick(wxCommandEvent &event)
{
    Cancel();
}

int ProgressDialog::ShowModalDetector::FilterEvent(wxEvent &event)
{
    const auto type = event.GetEventType();
    if (wxEVT_SHOW == type ||
        wxEVT_TIMER == type && ID_SHOW_TIMER == event.GetId())
    {
        if (!m_progressDialog.m_isClosed)
        {
            m_isShow = true;
        }
    }
    else if (wxEVT_ASYNC_METHOD_CALL == type)
    {
        return Event_Skip;
    }
    return Event_Ignore;
}

void ProgressDialog::ShowModalDetector::RemoveFilter()
{
    if (!m_isRemoved)
    {
        wxEvtHandler::RemoveFilter(this);
        m_isRemoved = true;
    }
}
