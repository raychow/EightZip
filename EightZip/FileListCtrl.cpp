#include "stdwx.h"
#include "FileListCtrl.h"

using namespace std;

FileListCtrl::FileListCtrl(
    wxWindow *parent,
    wxWindowID id /*= wxID_ANY*/,
    const wxPoint& pos /*= wxDefaultPosition*/,
    const wxSize& size /*= wxDefaultSize*/,
    long style /*= wxLC_REPORT | wxLC_VIRTUAL*/,
    const wxValidator& validator /*= wxDefaultValidator*/,
    const wxString& name /*= wxListCtrlNameStr*/)
    : wxListCtrl(parent, id, pos, size, style, validator, name)
    , m_imageList(true)
{
    Bind(wxEVT_ERASE_BACKGROUND, &FileListCtrl::__OnEraseBackground, this);
    Bind(wxEVT_LIST_COL_CLICK, &FileListCtrl::__OnListColumnClick, this);
}

void FileListCtrl::SetModel(shared_ptr<IModel> spModel
    , TString tstrFocused/* = wxEmptyString*/)
{
    typeid(m_spModel.get()).name();
    m_spModel = spModel;
    ClearAll();
    wxString wxstrColumnName;
    const auto &supportedItems = spModel->GetSupportedItems();
    for (auto t : supportedItems)
    {
        AppendColumn(
            GetColumnCaption(t), GetColumnFormat(t), GetColumnWidth(t));
    }
    const auto &children = m_spModel->GetEntries();
    m_vnChildrenMap.resize(children.size());
    int i = 0;
    int nSelectedIndex = -1;
    generate(m_vnChildrenMap.begin(), m_vnChildrenMap.end(), [&]() {
        if (!tstrFocused.empty() && children[i]->GetName() == tstrFocused)
        {
            nSelectedIndex = i;
        }
        return i++;
    });
    SetItemCount(children.size());
    if (-1 != nSelectedIndex)
    {
        SetItemState(
            nSelectedIndex, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
    }
    SetImageList(&m_imageList, wxIMAGE_LIST_SMALL);
    auto &sortParameter = m_mSortParameter[typeid(*spModel).name()];
    Sort(sortParameter.Column, sortParameter.IsAscending, true);
}

void FileListCtrl::Sort(int nColumn, bool isAscending, bool isForce /*= false*/)
{
    auto nItemCount = GetItemCount();
    vector<bool> vbSelectedIndex(nItemCount);
    long lItemIndex = -1;
    while (true)
    {
        lItemIndex = GetNextItem(
            lItemIndex, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
        if (-1 == lItemIndex)
        {
            break;
        }
        SetItemState(
            lItemIndex, 0, wxLIST_STATE_SELECTED | wxLIST_STATE_FOCUSED);
        vbSelectedIndex[m_vnChildrenMap[lItemIndex]] = true;
    }
    auto itemType = m_spModel->GetSupportedItems()[nColumn];
    bool isReverse = false;
    if (!isForce && m_currentSortParameter.Column == nColumn)
    {
        if (m_currentSortParameter.IsAscending == isAscending)
        {
            return;
        }
        else
        {
            isReverse = true;
        }
    }
    else
    {
        m_currentSortParameter.Column = nColumn;
    }
    m_currentSortParameter.IsAscending = isAscending;

    if (isReverse)
    {
        reverse(m_vnChildrenMap.begin(), m_vnChildrenMap.end());
    }
    else
    {
        const auto &children = m_spModel->GetEntries();
        sort(
            m_vnChildrenMap.begin(),
            m_vnChildrenMap.end(),
            [this, &itemType, &children](int nLeft, int nRight)
        {
            const auto &leftChild = children[nLeft];
            const auto &rightChild = children[nRight];
            bool isLeftChildDiectory = leftChild->IsDirectory();
            bool isRightChildDiectory = rightChild->IsDirectory();
            bool result = false;
            if (isLeftChildDiectory == isRightChildDiectory)
            {
                result = leftChild->Compare(
                    *rightChild, itemType, m_currentSortParameter.IsAscending);
            }
            else
            {
                result = isLeftChildDiectory
                    ^ !m_currentSortParameter.IsAscending;
            }

            return result;
        }
        );
    }

    bool isFirst = true;
    for (int i = 0; i != nItemCount; ++i)
    {
        if (vbSelectedIndex[m_vnChildrenMap[i]])
        {
            if (isFirst)
            {
                EnsureVisible(i);
                isFirst = false;
            }
            SetItemState(i, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
        }
    }

    Refresh(false);
}

int FileListCtrl::GetEntryIndex(int nListItemIndex) const
{
    if (0 > nListItemIndex || static_cast<int>(m_vnChildrenMap.size()) <= nListItemIndex)
    {
        return -1;
    }
    return m_vnChildrenMap.at(nListItemIndex);
}

wxString FileListCtrl::OnGetItemText(long item, long column) const
{
    try
    {
        const auto &spChild = m_spModel->GetEntries().at(m_vnChildrenMap[item]);
        return spChild->GetItem(m_spModel->GetSupportedItems().at(column));
    }
    catch (const out_of_range &)
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
    catch (const out_of_range &)
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

void FileListCtrl::__OnEraseBackground(wxEraseEvent &event)
{
    // to prevent flickering, erase only content *outside* of the 
    // actual list items stuff
    if (GetItemCount() > 0)
    {
        wxDC * dc = event.GetDC();
        assert(dc);

        // get some info
        wxCoord width = 0, height = 0;
        GetClientSize(&width, &height);

        wxCoord x, y, w, h;
        dc->SetClippingRegion(0, 0, width, height);
        dc->GetClippingBox(&x, &y, &w, &h);

        long top_item = GetTopItem();
        long bottom_item = top_item + GetCountPerPage();
        if (bottom_item >= GetItemCount()) {
            bottom_item = GetItemCount() - 1;
        }

        // trick: we want to exclude a couple pixels
        // on the left side thus use wxLIST_RECT_LABEL
        // for the top rect and wxLIST_RECT_BOUNDS for bottom rect
        wxRect top_rect, bottom_rect;
        GetItemRect(top_item, top_rect, wxLIST_RECT_LABEL);
        GetItemRect(bottom_item, bottom_rect, wxLIST_RECT_BOUNDS);

        // set the new clipping region and do erasing
        wxRect items_rect(top_rect.GetLeftTop(), bottom_rect.GetBottomRight());
        wxRegion reg(wxRegion(x, y, w, h));
        reg.Subtract(items_rect);
        dc->DestroyClippingRegion();
        dc->SetDeviceClippingRegion(reg);

        // do erasing
        dc->SetBackground(wxBrush(GetBackgroundColour(), wxSOLID));
        dc->Clear();

        // restore old clipping region
        dc->DestroyClippingRegion();
        dc->SetDeviceClippingRegion(wxRegion(x, y, w, h));
    }
    else
    {
        event.Skip();
    }
}

void FileListCtrl::__OnListColumnClick(wxListEvent &event)
{
    int nColumn = event.GetColumn();
    auto &sortParameter = m_mSortParameter[typeid(*m_spModel).name()];
    sortParameter.IsAscending = nColumn == sortParameter.Column
        ? !sortParameter.IsAscending
        : false;
    sortParameter.Column = nColumn;
    Sort(nColumn, sortParameter.IsAscending);
}
