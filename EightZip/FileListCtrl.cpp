#include "stdwx.h"
#include "FileListCtrl.h"

using namespace std;

FileListCtrl::FileListCtrl(wxWindow *parent, wxWindowID id /*= wxID_ANY*/, const wxPoint& pos /*= wxDefaultPosition*/, const wxSize& size /*= wxDefaultSize*/, long style /*= wxLC_REPORT | wxLC_VIRTUAL*/, const wxValidator& validator /*= wxDefaultValidator*/, const wxString& name /*= wxListCtrlNameStr*/)
    : wxListCtrl(parent, id, pos, size, style, validator, name)
    , m_imageList(true)
{
    Bind(wxEVT_LIST_COL_CLICK, &FileListCtrl::__OnListColumnClick, this);
}

void FileListCtrl::SetModel(shared_ptr<IModel> spModel)
{
    m_spModel = spModel;
    ClearAll();
    wxString wxstrColumnName;
    const auto &supportedItems = spModel->GetSupportedItems();
    for (auto t : supportedItems)
    {
        AppendColumn(GetColumnCaption(t), GetColumnFormat(t), GetColumnWidth(t));
    }
    const auto &children = m_spModel->GetEntries();
    m_vnChildrenMap.resize(children.size());
    int i = 0;
    generate(m_vnChildrenMap.begin(), m_vnChildrenMap.end(), [&i]() {
        return i++;
    });
    SetItemCount(children.size());
    SetImageList(&m_imageList, wxIMAGE_LIST_SMALL);
    m_nSortColumn = -1;
    Sort(0, true);
}

void FileListCtrl::Sort(int nColumn, bool isAscending)
{
    auto itemType = m_spModel->GetSupportedItems()[nColumn];
    bool isReverse = false;
    if (m_nSortColumn == nColumn)
    {
        if (m_isSortAscending ^ isAscending)
        {
            isReverse = true;
        }
        else
        {
            return;
        }
    }
    else
    {
        m_nSortColumn = nColumn;
    }
    m_isSortAscending = isAscending;

    if (isReverse)
    {
        reverse(m_vnChildrenMap.begin(), m_vnChildrenMap.end());
    }
    else
    {
        const auto &children = m_spModel->GetEntries();
        sort(m_vnChildrenMap.begin(), m_vnChildrenMap.end(), [this, isAscending, itemType, &children](int nLeft, int nRight) {
            const auto &leftChild = children[nLeft];
            const auto &rightChild = children[nRight];
            bool isLeftChildDiectory = leftChild->IsDirectory();
            bool isRightChildDiectory = rightChild->IsDirectory();
            bool result = false;
            if (isLeftChildDiectory ^ isRightChildDiectory)
            {
                result = isLeftChildDiectory ^ !isAscending;
            }
            else
            {
                result = leftChild->Compare(*rightChild, itemType, isAscending);
            }

            return result;
        });
    }

    Refresh(false);
}

int FileListCtrl::GetModelIndex(int nListItemIndex) const
{
    return m_vnChildrenMap.at(nListItemIndex);
}

wxString FileListCtrl::OnGetItemText(long item, long column) const
{
    try
    {
        const auto &spChild = m_spModel->GetEntries().at(m_vnChildrenMap[item]);
        return spChild->GetItem(m_spModel->GetSupportedItems().at(column));
    }
    catch (out_of_range)
    {
        return wxEmptyString;
    }
}

int FileListCtrl::OnGetItemImage(long item) const
{
    const auto &children = m_spModel->GetEntries();
    try
    {
        const auto spChild = children.at(m_vnChildrenMap[item]);
        return spChild->GetIconIndex();
    }
    catch (out_of_range)
    {
        return -1;
    }
}

wxString FileListCtrl::GetColumnCaption(IEntry::ItemType itemType)
{
    switch (itemType)
    {
    case IEntry::ItemType::Name:
        return _("Name");
    case IEntry::ItemType::Size:
        return _("Size");
    case IEntry::ItemType::PackedSize:
        return _("Packed Size");
    case IEntry::ItemType::TotalSize:
        return _("Total Size");
    case IEntry::ItemType::FreeSpace:
        return _("Free Space");
    case IEntry::ItemType::Type:
        return _("Type");
    case IEntry::ItemType::Modified:
        return _("Modified");
    case IEntry::ItemType::Created:
        return _("Created");
    case IEntry::ItemType::Accessed:
        return _("Accessed");
    case IEntry::ItemType::Attributes:
        return _("Attributes");
    case IEntry::ItemType::Comment:
        return _("Comment");
    case IEntry::ItemType::Encrypted:
        return _("Encrypted");
    case IEntry::ItemType::Method:
        return _("Method");
    case IEntry::ItemType::Block:
        return _("Block");
    case IEntry::ItemType::Folders:
        return _("Folders");
    case IEntry::ItemType::Files:
        return _("Files");
    case IEntry::ItemType::CRC:
        return _("CRC");
    default:
        return _("Unknown");
    }
}

wxListColumnFormat FileListCtrl::GetColumnFormat(IEntry::ItemType itemType)
{
    switch (itemType)
    {
    case IEntry::ItemType::Size:
    case IEntry::ItemType::PackedSize:
    case IEntry::ItemType::TotalSize:
    case IEntry::ItemType::FreeSpace:
    case IEntry::ItemType::Attributes:
    case IEntry::ItemType::Encrypted:
    case IEntry::ItemType::Block:
    case IEntry::ItemType::Folders:
    case IEntry::ItemType::Files:
    case IEntry::ItemType::CRC:
        return wxLIST_FORMAT_RIGHT;
    default:
        return wxLIST_FORMAT_LEFT;
    }
}

int FileListCtrl::GetColumnWidth(IEntry::ItemType itemType)
{
    switch (itemType)
    {
    case IEntry::ItemType::Name:
        return 300;
    case IEntry::ItemType::Size:
    case IEntry::ItemType::PackedSize:
    case IEntry::ItemType::Attributes:
    case IEntry::ItemType::Comment:
    case IEntry::ItemType::Encrypted:
    case IEntry::ItemType::Method:
    case IEntry::ItemType::Block:
    case IEntry::ItemType::Folders:
    case IEntry::ItemType::Files:
    case IEntry::ItemType::CRC:
        return 100;
    case IEntry::ItemType::Modified:
    case IEntry::ItemType::Created:
    case IEntry::ItemType::Accessed:
        return 150;
    default:
        return 100;
    }
}

void FileListCtrl::__OnListColumnClick(wxListEvent &event)
{
    int nColumn = event.GetColumn();

    Sort(nColumn, !m_isSortAscending);
}