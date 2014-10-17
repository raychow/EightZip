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

private:
    void __Create(TString tstrPath,
        boost::optional<time_t> oftExistModified,
        boost::optional<UINT64> oun64ExistSize,
        boost::optional<time_t> oftNewModified,
        boost::optional<UINT64> oun64NewSize);

    SystemImageList m_imageList;

};

#endif // OVERWRITEDIALOG_H
