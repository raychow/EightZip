#include "stdwx.h"
#include "FileListCtrl.h"

#include "FileTypeCache.h"

using namespace std;

wxBEGIN_EVENT_TABLE(FileListCtrl, wxListCtrl)
wxEND_EVENT_TABLE()

FileListCtrl::FileListCtrl(wxWindow *parent, wxWindowID id /*= wxID_ANY*/, const wxPoint& pos /*= wxDefaultPosition*/, const wxSize& size /*= wxDefaultSize*/, long style /*= wxLC_REPORT | wxLC_VIRTUAL*/, const wxValidator& validator /*= wxDefaultValidator*/, const wxString& name /*= wxListCtrlNameStr*/)
    : wxListCtrl(parent, id, pos, size, style, validator, name)
    , m_imageList(true)
{
    Bind(wxEVT_LIST_COL_CLICK, &FileListCtrl::__OnListColClick, this);
}

void FileListCtrl::SetModel(shared_ptr<IModel> spModel)
{
    m_spModel = spModel;
    ClearAll();
    wxString wxstrColumnName;
    const auto &supportedItems = spModel->GetChildrenSupportedItems();
    for (auto t : supportedItems)
    {
        AppendColumn(GetColumnCaption(t), GetColumnFormat(t), GetColumnWidth(t));
    }
    const auto &children = m_spModel->GetChildren();
    m_vnChildrenMap.resize(children.size());
    int i = 0;
    generate(m_vnChildrenMap.begin(), m_vnChildrenMap.end(), [&i]() {
        return i++;
    });
    SetItemCount(children.size());
    SetImageList(&m_imageList, wxIMAGE_LIST_SMALL);
}

wxString FileListCtrl::OnGetItemText(long item, long column) const
{
    const auto &children = m_spModel->GetChildren();
    try
    {
        const auto &spChild = children.at(m_vnChildrenMap[item]);
        const auto &supportedItems = spChild->GetChildrenSupportedItems();
        return spChild->GetItem(spChild->GetChildrenSupportedItems().at(column));
    }
    catch (out_of_range)
    {
        return wxEmptyString;
    }
}

int FileListCtrl::OnGetItemImage(long item) const
{
    const auto &children = m_spModel->GetChildren();
    try
    {
        const auto spChild = children.at(m_vnChildrenMap[item]);
        return FileTypeCache::GetInfo(spChild->IsDirectory(), spChild->GetFullPath()).GetIconIndex();
    }
    catch (out_of_range)
    {
        return -1;
    }
}

wxString FileListCtrl::GetColumnCaption(IModel::ItemType itemType)
{
    switch (itemType)
    {
    case IModel::ItemType::Name:
        return _("Name");
    case IModel::ItemType::Size:
        return _("Size");
    case IModel::ItemType::PackedSize:
        return _("Packed Size");
    case IModel::ItemType::Type:
        return _("Type");
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
    case IModel::ItemType::Name:
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
    case IModel::ItemType::Name:
        return 300;
    case IModel::ItemType::Size:
    case IModel::ItemType::PackedSize:
    case IModel::ItemType::Attributes:
    case IModel::ItemType::Comment:
    case IModel::ItemType::Encrypted:
    case IModel::ItemType::Method:
    case IModel::ItemType::Block:
    case IModel::ItemType::Folders:
    case IModel::ItemType::Files:
    case IModel::ItemType::CRC:
        return 100;
    case IModel::ItemType::Modified:
    case IModel::ItemType::Created:
    case IModel::ItemType::Accessed:
        return 150;
    default:
        return 100;
    }
}

void FileListCtrl::__OnListColClick(wxListEvent &event)
{
    int nColumn = event.GetColumn();
    auto itemType =m_spModel->GetChildrenSupportedItems()[nColumn];
    if (m_nSortColumn == nColumn)
    {
        m_isSortAscend = !m_isSortAscend;
    }
    else
    {
        m_nSortColumn = nColumn;
    }

    sort(m_vnChildrenMap.begin(), m_vnChildrenMap.end(), [this, itemType](int nLeft, int nRight) {
        const auto &children = m_spModel->GetChildren();
        const auto &leftChild = children[nLeft];
        const auto &rightChild = children[nRight];
        bool isLeftChildDiectory = leftChild->IsDirectory();
        bool isRightChildDiectory = rightChild->IsDirectory();
        bool result = false;
        if (isLeftChildDiectory != isRightChildDiectory)
        {
            result = isLeftChildDiectory ^ !m_isSortAscend;
        }
        else
        {

            if (m_isSortAscend)
            {
                result = leftChild->GetItem(itemType) < rightChild->GetItem(itemType);
            }
            else
            {
                result = leftChild->GetItem(itemType) > rightChild->GetItem(itemType);
            }
        }

        return result;
    });

    Refresh(false);
}
