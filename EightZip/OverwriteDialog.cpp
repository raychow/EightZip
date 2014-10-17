#include "stdwx.h"
#include "OverwriteDialog.h"

#include <iomanip>
#include <sstream>

#include "FileInfo.h"

using namespace std;

template<class T>
TString FormatWithCommas(T value)
{
    TStringStream tss;
    tss.imbue(locale(""));
    tss << fixed << value;
    return tss.str();
}

TString FormatDateTime(time_t value)
{
    TStringStream tss;
    tss.imbue(locale(""));
    tss << put_time(localtime(&value), wxT("%c"));
    return tss.str();
}

OverwriteDialog::OverwriteDialog(
    wxWindow *parent,
    wxWindowID id,
    const wxString& title,
    TString tstrPath,
    boost::optional<time_t> oftExistModified,
    boost::optional<UINT64> oun64ExistSize,
    boost::optional<time_t> oftNewModified,
    boost::optional<UINT64> oun64NewSize,
    const wxPoint& pos /*= wxDefaultPosition*/,
    const wxSize& size /*= wxDefaultSize*/,
    long style /*= wxDEFAULT_DIALOG_STYLE*/,
    const wxString& name /*= wxDialogNameStr*/)
    : wxDialog(parent, id, title, pos, size, style, name)
    , m_imageList(false)
{
    __Create(tstrPath,
        oftExistModified, oun64ExistSize, oftNewModified, oun64NewSize);
}

OverwriteDialog::~OverwriteDialog()
{

}

void OverwriteDialog::__Create(TString tstrPath,
    boost::optional<time_t> oftExistModified,
    boost::optional<UINT64> oun64ExistSize,
    boost::optional<time_t> oftNewModified,
    boost::optional<UINT64> oun64NewSize)
{
    auto *pSizerMain = new wxBoxSizer(wxVERTICAL);

    auto *pSizerFileInfo = new wxStaticBoxSizer(
        new wxStaticBox(this, wxID_ANY, wxEmptyString), wxVERTICAL);

    pSizerFileInfo->AddSpacer(6);
    pSizerFileInfo->Add(new wxStaticText(this, wxID_ANY,
        _("The following file already exists:")),
        wxSizerFlags().Border(wxBOTTOM, 3));
    auto pTextCtrlPath = new wxTextCtrl(this, wxID_ANY, tstrPath,
        wxDefaultPosition, wxSize(350, 60), wxTE_MULTILINE);
    pTextCtrlPath->SetEditable(false);
    pSizerFileInfo->Add(pTextCtrlPath,
        wxSizerFlags().Border(wxBOTTOM, 6).Expand());

    pSizerFileInfo->Add(new wxStaticText(this, wxID_ANY,
        _("Would you like to replace the existing file")));

    auto *pSizerExistFile = new wxBoxSizer(wxHORIZONTAL);
    pSizerExistFile->Add(new wxStaticBitmap(this, wxID_ANY,
        m_imageList.GetIcon(FileInfo::GetIconIndex(tstrPath))),
        wxSizerFlags().Border(wxRIGHT, 6));
    auto *pSizerExistFileInfo = new wxBoxSizer(wxVERTICAL);
    if (oun64ExistSize)
    {
        pSizerExistFileInfo->Add(new wxStaticText(this, wxID_ANY,
            wxString::Format(_("%s bytes"),
            FormatWithCommas(*oun64ExistSize))));
    }
    if (oftExistModified)
    {
        TStringStream tss;
        tss << put_time(localtime(&*oftExistModified), wxT("%c"));
        pSizerExistFileInfo->Add(new wxStaticText(this, wxID_ANY,
            wxString::Format(_("modified on %s"),
            FormatDateTime(*oftExistModified))));
    }
    pSizerExistFile->Add(pSizerExistFileInfo);
    pSizerFileInfo->Add(pSizerExistFile,
        wxSizerFlags().Border(wxTOP | wxBOTTOM, 6));

    pSizerFileInfo->Add(new wxStaticText(this, wxID_ANY,
        _("with this one?")));

    auto *pSizerNewFile = new wxBoxSizer(wxHORIZONTAL);
    pSizerNewFile->Add(new wxStaticBitmap(this, wxID_ANY,
        m_imageList.GetIcon(FileInfo::GetIconIndex(tstrPath))),
        wxSizerFlags().Border(wxRIGHT, 6));
    auto *pSizerNewFileInfo = new wxBoxSizer(wxVERTICAL);
    if (oun64NewSize)
    {
        pSizerNewFileInfo->Add(new wxStaticText(this, wxID_ANY,
            wxString::Format(_("%s bytes"),
            FormatWithCommas(*oun64NewSize))));
    }
    if (oftNewModified)
    {
        pSizerNewFileInfo->Add(new wxStaticText(this, wxID_ANY,
            wxString::Format(_("modified on %s"),
            FormatDateTime(*oftNewModified))));
    }
    pSizerNewFile->Add(pSizerNewFileInfo);
    pSizerFileInfo->Add(pSizerNewFile,
        wxSizerFlags().Border(wxTOP | wxBOTTOM, 6));

    pSizerMain->Add(pSizerFileInfo,
        wxSizerFlags().Expand().Border(wxLEFT | wxRIGHT | wxBOTTOM, 8));

    auto *pSizerButton = new wxGridSizer(2, 4, 3, 3);

    auto pButtonYes = new wxButton(this, wxID_ANY, _("&Yes"));
    pSizerButton->Add(pButtonYes);
    auto pButtonNo = new wxButton(this, wxID_ANY, _("&No"));
    pSizerButton->Add(pButtonNo);
    auto pButtonRename = new wxButton(this, wxID_ANY, _("&Rename"));
    pSizerButton->Add(pButtonRename);
    pSizerButton->Add(new wxWindow());
    auto pButtonYesToAll = new wxButton(this, wxID_ANY, _("Yes to &All"));
    pSizerButton->Add(pButtonYesToAll);
    auto pButtonNoToAll = new wxButton(this, wxID_ANY, _("No to A&ll"));
    pSizerButton->Add(pButtonNoToAll);
    auto pButtonRenameAll = new wxButton(this, wxID_ANY, _("R&ename All"));
    pSizerButton->Add(pButtonRenameAll);
    auto pButtonCancel = new wxButton(this, wxID_ANY, _("&Cancel"));
    pSizerButton->Add(pButtonCancel);

    pSizerMain->Add(pSizerButton,
        wxSizerFlags().Border(wxBOTTOM, 8).Center());

    SetSizerAndFit(pSizerMain);
}