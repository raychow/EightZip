#include "stdwx.h"
#include "FileListCtrl.h"

using namespace std;

wxBEGIN_EVENT_TABLE(FileListCtrl, wxListCtrl)
wxEND_EVENT_TABLE()

FileListCtrl::FileListCtrl(wxWindow *parent, wxWindowID id /*= wxID_ANY*/, const wxPoint& pos /*= wxDefaultPosition*/, const wxSize& size /*= wxDefaultSize*/, long style /*= wxLC_REPORT | wxLC_VIRTUAL*/, const wxValidator& validator /*= wxDefaultValidator*/, const wxString& name /*= wxListCtrlNameStr*/)
    : wxListCtrl(parent, id, pos, size, style, validator, name)
    , m_imageList(true)
{

}

void FileListCtrl::SetModel(shared_ptr<IModel> spModel)
{
    m_spModel = spModel;
    ClearAll();
    AppendColumn(_T("Name"), wxLIST_FORMAT_LEFT, 300);
    wxString wxstrColumnName;
    for (auto t : spModel->GetChildrenSupportedItems())
    {
        AppendColumn(GetColumnCaption(t), GetColumnFormat(t), GetColumnWidth(t));
    }
    SetItemCount(m_spModel->GetDirChildren().size() + m_spModel->GetFileChildren().size());
    SetImageList(&m_imageList, wxIMAGE_LIST_SMALL);
}

wxString FileListCtrl::OnGetItemText(long item, long column) const
{
    const auto *pChildren = &m_spModel->GetDirChildren();
    int nDirChildren = pChildren->size();
    if (nDirChildren <= item)
    {
        pChildren = &m_spModel->GetFileChildren();
        item -= nDirChildren;
    }
    if (0 == column)
    {
        return pChildren->at(item)->GetName();
    }
    else
    {
        --column;
        return pChildren->at(item)->GetItem((IModel::ItemType)column);
    }
}

int FileListCtrl::OnGetItemImage(long item) const
{
    const auto *pChildren = &m_spModel->GetDirChildren();
    int nDirChildren = pChildren->size();
    if (nDirChildren <= item)
    {
        pChildren = &m_spModel->GetFileChildren();
        item -= nDirChildren;
    }
    wxFileName info(pChildren->at(item)->GetFullPath());
    return SystemImageList::GetIconIndex(info.DirExists(), info.GetFullPath());
}

wxString FileListCtrl::GetColumnCaption(IModel::ItemType itemType)
{
    switch (itemType)
    {
    case IModel::ItemType::Size:
        return _("Size");
    case IModel::ItemType::PackedSize:
        return _("Packed Size");
    case IModel::ItemType::Modified:
        return _("Modified");
    case IModel::ItemType::Created:
        return _("Created");
    case IModel::ItemType::Accessed:
        return _("Accessed");
    case IModel::ItemType::Attributes:
        return _("Attributes");
    case IModel::ItemType::Comment:
        return _("Comment");
    case IModel::ItemType::Encrypted:
        return _("Encrypted");
    case IModel::ItemType::Method:
        return _("Method");
    case IModel::ItemType::Block:
        return _("Block");
    case IModel::ItemType::Folders:
        return _("Folders");
    case IModel::ItemType::Files:
        return _("Files");
    case IModel::ItemType::CRC:
        return _("CRC");
    default:
        return _("Unknown");
    }
}

wxListColumnFormat FileListCtrl::GetColumnFormat(IModel::ItemType itemType)
{
    switch (itemType)
    {
    case IModel::ItemType::Modified:
    case IModel::ItemType::Created:
    case IModel::ItemType::Accessed:
    case IModel::ItemType::Comment:
    case IModel::ItemType::Method:
        return wxLIST_FORMAT_LEFT;
    case IModel::ItemType::Size:
    case IModel::ItemType::PackedSize:
    case IModel::ItemType::Attributes:
    case IModel::ItemType::Encrypted:
    case IModel::ItemType::Block:
    case IModel::ItemType::Folders:
    case IModel::ItemType::Files:
    case IModel::ItemType::CRC:
        return wxLIST_FORMAT_RIGHT;
    default:
        return wxLIST_FORMAT_LEFT;
    }
}

int FileListCtrl::GetColumnWidth(IModel::ItemType itemType)
{
    switch (itemType)
    {
    case IModel::ItemType::Size:
    case IModel::ItemType::PackedSize:
    case IModel::ItemType::Modified:
    case IModel::ItemType::Created:
    case IModel::ItemType::Accessed:
    case IModel::ItemType::Attributes:
    case IModel::ItemType::Comment:
    case IModel::ItemType::Encrypted:
    case IModel::ItemType::Method:
    case IModel::ItemType::Block:
    case IModel::ItemType::Folders:
    case IModel::ItemType::Files:
    case IModel::ItemType::CRC:
        return 100;
    default:
        return 100;
    }
}