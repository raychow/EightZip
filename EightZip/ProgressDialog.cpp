#include "stdwx.h"
#include "ProgressDialog.h"

#include "SevenZipCore/CommonHelper.h"

using namespace std;

ProgressDialog::ProgressDialog(
    wxWindow *parent,
    wxWindowID id,
    const wxString& title,
    const wxPoint& pos /*= wxDefaultPosition*/,
    const wxSize& size /*= wxDefaultSize*/,
    long style /*= wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER*/,
    const wxString& name /*= wxDialogNameStr*/)
    : wxDialog(parent, id, title, pos, size, style, name)
{
    __Create();
    __StartTimer();
}

void ProgressDialog::SetArchivePath(const TString &tstrPath)
{
    lock_guard<mutex> lg(m_mutex);
    m_tstrArchiveFileName = SevenZipCore::Helper::GetFileName(tstrPath);
    m_tstrArchivePath = tstrPath;
}

void ProgressDialog::SetCurrentFile(const TString &tstrFileName)
{
    lock_guard<mutex> lg(m_mutex);
    m_tstrCurrentFile = tstrFileName;
}

void ProgressDialog::SetTotal(UINT64 un64Total)
{
    lock_guard<mutex> lg(m_mutex);
    m_un64Total = un64Total;
}

void ProgressDialog::SetCompleted(UINT64 un64Completed)
{
    lock_guard<mutex> lg(m_mutex);
    m_un64Completed = un64Completed;
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
    pSizerStatus->Add(m_pLabelArchivePath);

    auto *pSizerCurrentFileName = new wxBoxSizer(wxHORIZONTAL);
    m_pLabelCurrentFile = new wxStaticText(this, wxID_ANY, "%s");
    m_pLabelCurrentFile->SetWindowStyle(
        wxST_NO_AUTORESIZE | wxST_ELLIPSIZE_END);
    pSizerCurrentFileName->Add(
        m_pLabelCurrentFile, wxSizerFlags().Proportion(1));
    pSizerStatus->Add(pSizerCurrentFileName, wxSizerFlags().Expand());

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
    pSizerButton->Add(new wxButton(this, wxID_ANY, _("&Background")),
        wxSizerFlags().Border(wxRIGHT, 5));
    pSizerButton->Add(new wxButton(this, wxID_ANY, _("&Pause")),
        wxSizerFlags().Border(wxRIGHT, 5));
    pSizerButton->Add(new wxButton(this, wxID_CANCEL, _("Cancel")),
        wxSizerFlags());
    pSizerMain->Add(pSizerButton,
        wxSizerFlags().Border(wxALL, 7).Right());

    SetSizer(pSizerMain);
    Fit();
}

void ProgressDialog::__StartTimer()
{
    m_tpStart = chrono::system_clock::now();
    m_timer.Bind(wxEVT_TIMER, &ProgressDialog::__Update, this);
    m_timer.Start(UPDATE_INTERVAL);
}

void ProgressDialog::__Update(wxTimerEvent &WXUNUSED(event))
{
    lock_guard<mutex> lg(m_mutex);
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

    SetTitle(wxString::Format(_("Extracting from %s"),
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

