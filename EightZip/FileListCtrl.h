// FileListCtrl.h

#ifndef FILELISTCTRL_H
#define FILELISTCTRL_H

#include "IModel.h"
#include "SystemImageList.h"

class FileListCtrl
    : public wxListCtrl
{
public:
    FileListCtrl(wxWindow *parent,
        wxWindowID id = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxLC_REPORT | wxLC_VIRTUAL,
        const wxValidator& validator = wxDefaultValidator,
        const wxString& name = wxListCtrlNameStr);

    void SetModel(std::shared_ptr<IModel> spModel);

    static wxString GetColumnCaption(IModel::ItemType itemType);
    static wxListColumnFormat GetColumnFormat(IModel::ItemType itemType);
    static int GetColumnWidth(IModel::ItemType itemType);
protected:
    virtual wxString OnGetItemText(long item, long column) const;
    virtual int OnGetItemImage(long item) const;

private:
    std::shared_ptr<IModel> m_spModel;
    SystemImageList m_imageList;

    DECLARE_EVENT_TABLE()
};

#endif // FILELISTCTRL_H
