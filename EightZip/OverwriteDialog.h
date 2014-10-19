// OverwriteDialog.h

#ifndef OVERWRITEDIALOG_H
#define OVERWRITEDIALOG_H

#include <boost/optional.hpp>

#include "SevenZipCore/TString.h"

#include "SystemImageList.h"

class OverwriteDialog
    : public wxDialog
{
public:
    OverwriteDialog(
        wxWindow *parent,
        wxWindowID id,
        const wxString& title,
        TString tstrPath,
        boost::optional<time_t> oftExistModified,
        boost::optional<UINT64> oun64ExistSize,
        boost::optional<time_t> oftNewModified,
        boost::optional<UINT64> oun64NewSize,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_DIALOG_STYLE,
        const wxString& name = wxDialogNameStr);

    virtual ~OverwriteDialog();

    inline TString GetPath() const { return m_tstrPath; }

private:
    SystemImageList m_imageList;
    TString m_tstrPath;

    void __Create(
        boost::optional<time_t> oftExistModified,
        boost::optional<UINT64> oun64ExistSize,
        boost::optional<time_t> oftNewModified,
        boost::optional<UINT64> oun64NewSize);

};

#endif // OVERWRITEDIALOG_H
