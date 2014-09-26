#include "stdwx.h"
#include "ProgressDialog.h"

using namespace std;

ProgressDialog::ProgressDialog(
    wxWindow *parent,
    wxWindowID id,
    const wxString& title,
    const wxPoint& pos /*= wxDefaultPosition*/,
    const wxSize& size /*= wxDefaultSize*/,
    long style /*= wxDEFAULT_DIALOG_STYLE*/,
    const wxString& name /*= wxDialogNameStr*/)
    : wxDialog(parent, id, title, pos, size, style, name)
{
    __Create();
    __StartTimer();
}

void ProgressDialog::SetArchivePath(const TString &tstrPath)
{
    lock_guard<mutex> lg(m_mutex);
    m_tstrArchivePath = tstrPath;
    /*m_pLabelArchivePath->SetLabel(
    wxString::Format(_("Archive %s"), tstrPath));*/
}

void ProgressDialog::SetCurrentFile(const TString &tstrFileName)
{
    lock_guard<mutex> lg(m_mutex);
    m_tstrCurrentFile = tstrFileName;
}

void ProgressDialog::__Create()
{
    auto *pSizerMain = new wxBoxSizer(wxVERTICAL);
    
    auto *pSizerCurrentFile = new wxStaticBoxSizer(
        new wxStaticBox(this, wxID_ANY, wxEmptyString), wxVERTICAL);

    m_pLabelArchivePath = new wxStaticText(this, wxID_ANY, _("Archive %s"));
    pSizerCurrentFile->Add(m_pLabelArchivePath);

    auto *pSizerCurrentFileName = new wxBoxSizer(wxHORIZONTAL);
    m_pLabelCurrentFile = new wxStaticText(this, wxID_ANY, "%s");
    pSizerCurrentFileName->Add(m_pLabelCurrentFile,
        wxSizerFlags().Proportion(1));
    pSizerCurrentFileName->Add(new wxStaticText(this, wxID_ANY, "0%"));
    pSizerCurrentFile->Add(pSizerCurrentFileName, wxSizerFlags().Expand());

    m_pGaugeCurrentFile = new wxGauge(this, wxID_ANY, 100);
    pSizerCurrentFile->Add(m_pGaugeCurrentFile,
        wxSizerFlags().Expand());

    pSizerMain->AddSpacer(3);
    pSizerMain->Add(pSizerCurrentFile,
        wxSizerFlags().Expand().Border(wxLEFT | wxRIGHT, 8));


    auto *pSizerStatus = new wxStaticBoxSizer(
        new wxStaticBox(this, wxID_ANY, wxEmptyString), wxVERTICAL);

    auto *pSizerElapsedTime = new wxBoxSizer(wxHORIZONTAL);
    pSizerElapsedTime->Add(new wxStaticText(this, wxID_ANY, _("Elapsed time")),
        wxSizerFlags().Proportion(1));
    pSizerElapsedTime->Add(new wxStaticText(this, wxID_ANY, ("00:00:00")));
    pSizerStatus->Add(pSizerElapsedTime, wxSizerFlags().Expand());

    auto *pSizerTimeLeft = new wxBoxSizer(wxHORIZONTAL);
    pSizerTimeLeft->Add(new wxStaticText(this, wxID_ANY, _("Time left")),
        wxSizerFlags().Proportion(1));
    pSizerTimeLeft->Add(new wxStaticText(this, wxID_ANY, ("00:00:00")));
    pSizerStatus->Add(pSizerTimeLeft, wxSizerFlags().Expand());

    pSizerStatus->Add(new wxStaticLine(this),
        wxSizerFlags().Expand().Border(wxTOP | wxBOTTOM, 5));

    auto *pSizerProcessed = new wxBoxSizer(wxHORIZONTAL);
    pSizerProcessed->Add(new wxStaticText(this, wxID_ANY, _("Processed")),
        wxSizerFlags().Proportion(1));
    pSizerProcessed->Add(new wxStaticText(this, wxID_ANY, ("0%")));
    pSizerStatus->Add(pSizerProcessed, wxSizerFlags().Expand());

    m_pGaugeProcessed = new wxGauge(this, wxID_ANY, 100);
    pSizerStatus->Add(m_pGaugeProcessed,
        wxSizerFlags().Expand());
    pSizerMain->AddSpacer(3);
    pSizerMain->Add(pSizerStatus,
        wxSizerFlags().Expand().Border(wxLEFT | wxRIGHT, 8));

    auto *pSizerButton = new wxBoxSizer(wxHORIZONTAL);
    pSizerButton->Add(new wxButton(this, wxID_ANY, _("&Background")),
        wxSizerFlags().Left().Border(wxRIGHT, 5));
    pSizerButton->Add(new wxButton(this, wxID_ANY, _("&Pause")),
        wxSizerFlags().Center());
    pSizerButton->Add(new wxButton(this, wxID_CANCEL, _("Cancel")),
        wxSizerFlags().Right().Border(wxLEFT, 5));
    pSizerMain->Add(pSizerButton,
        wxSizerFlags().Border(wxALL, 7).Expand());

    SetSizer(pSizerMain);
    Fit();
}

void ProgressDialog::__StartTimer()
{
    m_timer.Bind(wxEVT_TIMER, &ProgressDialog::__Update, this);
    m_timer.Start(100);
}

void ProgressDialog::__Update(wxTimerEvent &WXUNUSED(event))
{
    lock_guard<mutex> lg(m_mutex);
    m_pLabelArchivePath->SetLabel(m_tstrArchivePath);
    m_pLabelCurrentFile->SetLabel(m_tstrCurrentFile);
}

