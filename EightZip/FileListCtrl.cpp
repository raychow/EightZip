#include "stdwx.h"
#include "FileListCtrl.h"

using namespace std;

#include "EntryBase.h"

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

void FileListCtrl::SetModel(ModelBase &model
    , TString tstrFocused/* = wxEmptyString*/)
{
    m_pModel = &model;
    ClearAll();
    wxString wxstrColumnName;
    const auto &supportedItems = m_pModel->GetSupportedItems();
    for (auto t : supportedItems)
    {
        AppendColumn(
            GetColumnCaption(t), GetColumnFormat(t), GetColumnWidth(t));
    }
    m_vnChildrenMap.resize(m_pModel->GetEntryCount());
    int i = 0;
    int nSelectedIndex = -1;
    generate(m_vnChildrenMap.begin(), m_vnChildrenMap.end(), [&]() {
        if (!tstrFocused.empty() && (*m_pModel)[i].GetName() == tstrFocused)
        {
            nSelectedIndex = i;
        }
        return i++;
    });
    SetItemCount(m_pModel->GetEntryCount());
    if (-1 != nSelectedIndex)
    {
        SetItemState(
            nSelectedIndex, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
    }
    SetImageList(&m_imageList, wxIMAGE_LIST_SMALL);
    auto &sortParameter = m_mSortParameter[typeid(*m_pModel).name()];
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
    auto itemType = m_pModel->GetSupportedItems()[nColumn];
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
        sort(
            m_vnChildrenMap.begin(),
            m_vnChildrenMap.end(),
            [this, &itemType](int nLeft, int nRight)
        {
            const auto &leftChild = (*m_pModel)[nLeft];
            const auto &rightChild = (*m_pModel)[nRight];
            bool isLeftChildDiectory = leftChild.IsDirectory();
            bool isRightChildDiectory = rightChild.IsDirectory();
            bool result = false;
            if (isLeftChildDiectory == isRightChildDiectory)
            {
                result = leftChild.Compare(
                    rightChild, itemType, m_currentSortParameter.IsAscending);
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
        return m_pModel->GetEntry(m_vnChildrenMap[item]).GetItem(
            m_pModel->GetSupportedItems().at(column));
    }
    catch (const out_of_range &)
    {
        return wxEmptyString;
    }
}

int FileListCtrl::OnGetItemImage(long item) const
{
    try
    {
        return m_pModel->GetEntry(m_vnChildrenMap[item]).GetIconIndex();
    }
    catch (const out_of_range &)
    {
        return -1;
    }
}

wxString FileListCtrl::GetColumnCaption(EntryItemType itemType)
{
    switch (itemType)
    {
    case EntryItemType::Name:
        return _("Name");
    case EntryItemType::Size:
        return _("Size");
    case EntryItemType::PackedSize:
        return _("Packed Size");
    case EntryItemType::TotalSize:
        return _("Total Size");
    case EntryItemType::FreeSpace:
        return _("Free Space");
    case EntryItemType::Type:
        return _("Type");
    case EntryItemType::Modified:
        return _("Modified");
    case EntryItemType::Created:
        return _("Created");
    case EntryItemType::Accessed:
        return _("Accessed");
    case EntryItemType::Attributes:
        return _("Attributes");
    case EntryItemType::Comment:
        return _("Comment");
    case EntryItemType::Encrypted:
        return _("Encrypted");
    case EntryItemType::Method:
        return _("Method");
    case EntryItemType::Block:
        return _("Block");
    case EntryItemType::Folders:
        return _("Folders");
    case EntryItemType::Files:
        return _("Files");
    case EntryItemType::CRC:
        return _("CRC");
    default:
        return _("Unknown");
    }
}

wxListColumnFormat FileListCtrl::GetColumnFormat(EntryItemType itemType)
{
    switch (itemType)
    {
    case EntryItemType::Size:
    case EntryItemType::PackedSize:
    case EntryItemType::TotalSize:
    case EntryItemType::FreeSpace:
    case EntryItemType::Attributes:
    case EntryItemType::Encrypted:
    case EntryItemType::Block:
    case EntryItemType::Folders:
    case EntryItemType::Files:
    case EntryItemType::CRC:
        return wxLIST_FORMAT_RIGHT;
    default:
        return wxLIST_FORMAT_LEFT;
    }
}

int FileListCtrl::GetColumnWidth(EntryItemType itemType)
{
    switch (itemType)
    {
    case EntryItemType::Name:
        return 300;
    case EntryItemType::Size:
    case EntryItemType::PackedSize:
    case EntryItemType::Attributes:
    case EntryItemType::Comment:
    case EntryItemType::Encrypted:
    case EntryItemType::Method:
    case EntryItemType::Block:
    case EntryItemType::Folders:
    case EntryItemType::Files:
    case EntryItemType::CRC:
        return 100;
    case EntryItemType::Modified:
    case EntryItemType::Created:
    case EntryItemType::Accessed:
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
    auto &sortParameter = m_mSortParameter[typeid(*m_pModel).name()];
    sortParameter.IsAscending = nColumn == sortParameter.Column
        ? !sortParameter.IsAscending
        : true;
    sortParameter.Column = nColumn;
    Sort(nColumn, sortParameter.IsAscending);
}
