// FileListCtrl.h

#ifndef FILELISTCTRL_H
#define FILELISTCTRL_H

#include <map>

#include "ModelBase.h"
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

    void SetModel(ModelBase &model, TString tstrFocused = wxEmptyString);

    static wxString GetColumnCaption(EntryItemType itemType);
    static wxListColumnFormat GetColumnFormat(EntryItemType itemType);
    static int GetColumnWidth(EntryItemType itemType);

    void Sort(int nColumn, bool isAscending, bool isForce = false);

    int GetEntryIndex(int nListItemIndex) const;

protected:
    virtual wxString OnGetItemText(long item, long column) const;
    virtual int OnGetItemImage(long item) const;

private:
    struct SortParameter
    {
        int Column = 0;
        bool IsAscending = true;
    };

    ModelBase *m_pModel = nullptr;
    SystemImageList m_imageList;
    std::vector<int> m_vnChildrenMap;

    std::map<std::string, SortParameter> m_mSortParameter;
    SortParameter m_currentSortParameter;

    // Flicker-free drawing.
    // See also: http://wiki.wxwidgets.org/Flicker-Free_Drawing
    void __OnEraseBackground(wxEraseEvent &event);

    void __OnListColumnClick(wxListEvent &event);

};

#endif // FILELISTCTRL_H
