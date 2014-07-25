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
    FileExplorer(wxWindow *parent,
        wxWindowID winid = wxID_ANY,
        const wxPoint &pos = wxDefaultPosition,
        const wxSize &size = wxDefaultSize,
        long style = wxTAB_TRAVERSAL | wxNO_BORDER,
        const wxString &name = wxPanelNameStr);
    virtual ~FileExplorer() { }

private:
    FileListCtrl *m_pListCtrl = nullptr;

    std::shared_ptr<IModel> m_spModel;

    void __Create();
    void __CreatePath(wxBoxSizer *pMainSizer);
    void __CreateExplorer(wxBoxSizer *pSizerMain);

    void __OnListCtrlActive(wxListEvent &event);
};

#endif // FILEEXPLORER_H
