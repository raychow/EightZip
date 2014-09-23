// FileExplorer.h

#ifndef FILEEXPLORER_H
#define FILEEXPLORER_H

#include <memory>

#include "ArchiveModel.h"
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
    virtual ~FileExplorer();

    void NavigateTo(TString tstrPath);

    std::shared_ptr<IModel> GetModel() const { return m_spModel; }
    
    int GetSelectedIndex() const;
    std::vector<int> GetSelectedIndexes() const;
    int GetSelectedEntryIndex() const;
    std::vector<int> GetSelectedEntryIndexes() const;
    std::shared_ptr<IEntry> GetSelectedEntry() const;
    std::shared_ptr<IEntry> GetEntry(int nIndex) const;
    bool CanExtract() const;

    void Extract(TString tstrPath);
    void Extract(TString tstrPath, std::shared_ptr<IModel> spModel);

private:
    wxToolBar *m_pParentFolderToolBar = nullptr;
    wxComboBox *m_pAddressComboBox = nullptr;
    FileListCtrl *m_pListCtrl = nullptr;

    std::shared_ptr<IModel> m_spModel;

    void __Create();
    void __CreateAddressBar(wxBoxSizer *pMainSizer);
    void __CreateExplorer(wxBoxSizer *pSizerMain);

    void __SetModel(std::shared_ptr<IModel> spModel
        , TString tstrFocusedName = wxEmptyString);

    void __OnParentFolderClick(wxCommandEvent &event);
    void __OnPathComboBoxKeyDown(wxKeyEvent& event);
    void __OnListItemActivated(wxListEvent &event);

};

#endif // FILEEXPLORER_H
