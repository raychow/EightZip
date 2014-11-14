// FileExplorer.h

#ifndef FILEEXPLORER_H
#define FILEEXPLORER_H

#include <memory>

#include "EntryBase.h"
#include "FileListCtrl.h"
#include "ModelBase.h"
#include "VirtualModel.h"

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

    std::shared_ptr<ModelBase> GetModel() const { return m_spModel; }
    
    int GetSelectedIndex() const;
    std::vector<int> GetSelectedIndexes() const;
    EntryBase &GetSelectedEntry() const;
    std::vector<std::reference_wrapper<EntryBase>> GetSelectedEntries() const;
    EntryBase &GetEntry(int nIndex) const;

    void Extract(TString tstrPath);

private:
    wxToolBar *m_pParentFolderToolBar = nullptr;
    wxComboBox *m_pAddressComboBox = nullptr;
    FileListCtrl *m_pListCtrl = nullptr;

    std::shared_ptr<ModelBase> m_spModel;

    void __Create();
    void __CreateAddressBar(wxBoxSizer *pMainSizer);
    void __CreateExplorer(wxBoxSizer *pSizerMain);

    void __SetModel(std::shared_ptr<ModelBase> spModel
        , TString tstrFocusedName = wxEmptyString);

    void __OnParentFolderClick(wxCommandEvent &event);
    void __OnPathComboBoxKeyDown(wxKeyEvent& event);
    void __OnListItemActivated(wxListEvent &event);

};

#endif // FILEEXPLORER_H
