// WindowStateManager.h

#ifndef WINDOWSTATEMANAGER_H
#define WINDOWSTATEMANAGER_H

#include "EightZipConfig.h"

class WindowStateManager
    : public wxEvtHandler
{
public:
    WindowStateManager();
    WindowStateManager(wxTopLevelWindowBase *pWindow,
        ConfigIndex indexLocationX,
        ConfigIndex indexLocationY,
        ConfigIndex indexWidth,
        ConfigIndex indexHeight,
        ConfigIndex indexIsMaximized);
    virtual ~WindowStateManager();

    void Create(wxTopLevelWindowBase *pWindow,
        ConfigIndex indexLocationX,
        ConfigIndex indexLocationY,
        ConfigIndex indexWidth,
        ConfigIndex indexHeight,
        ConfigIndex indexIsMaximized);

    void LoadState();
    void SaveState() const;
    void Destroy();

protected:
    void _OnMove(wxMoveEvent& event);
    void _OnSize(wxSizeEvent& event);

    void _UpdateLastLayout();

private:
    wxTopLevelWindowBase *m_pWindow = nullptr;
    ConfigIndex m_indexLocationX;
    ConfigIndex m_indexLocationY;
    ConfigIndex m_indexWidth;
    ConfigIndex m_indexHeight;
    ConfigIndex m_indexIsMaximized;

    bool m_isLastMaxmized = false;
    wxPoint m_lastPosition;
    wxSize m_lastSize;

    bool __IsConfigLocation() const
    {
        return ConfigIndex::Nothing != m_indexLocationX
            && ConfigIndex::Nothing != m_indexLocationY;
    }

    bool __IsConfigSize() const
    {
        return ConfigIndex::Nothing != m_indexWidth
            && ConfigIndex::Nothing != m_indexHeight
            && ConfigIndex::Nothing != m_indexIsMaximized;
    }

};

#endif // WINDOWSTATEMANAGER_H
