// FileExplorer.h

#ifndef FILEEXPLORER_H
#define FILEEXPLORER_H

#include <memory>

#include "FileListCtrl.h"
#include "IModel.h"

class FileExplorer
    : public wxPanel
{
public:
    enum
    {
        ID_PARENT_FOLDER = wxID_HIGHEST + 1,
    };

    FileExplorer(wxWindow *parent,
        wxWindowID winid = wxID_ANY,
        const wxPoint &pos = wxDefaultPosition,
        const wxSize &size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER,
        const wxString &name = wxPanelNameStr);
    virtual ~FileExplorer() {}

    void NavigateTo(TString tstrPath);

private:
    wxToolBar *m_pToolBar = nullptr;
    wxComboBox *m_pComboBox = nullptr;
    FileListCtrl *m_pListCtrl = nullptr;

    std::shared_ptr<IModel> m_spModel;

    void __Create();
    void __CreatePath(wxBoxSizer *pMainSizer);
    void __CreateExplorer(wxBoxSizer *pSizerMain);

    void __SetModel(std::shared_ptr<IModel> spModel);

    void __OnParentFolderClick(wxCommandEvent &event);
    void __OnPathComboBoxEnter(wxCommandEvent& event);
    void __OnListItemActivated(wxListEvent &event);

};

#endif // FILEEXPLORER_H
