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
    wxTopLevelWindowBase *m_pWindow = nullptr;
    ConfigIndex m_indexLocationX;
    ConfigIndex m_indexLocationY;
    ConfigIndex m_indexWidth;
    ConfigIndex m_indexHeight;
    ConfigIndex m_indexIsMaximized;

    bool m_isLastMaxmized = false;
    wxPoint m_lastPosition;
    wxSize m_lastSize;

    void _OnMove(wxMoveEvent& event);
    void _OnSize(wxSizeEvent& event);

    void _UpdateLastLayout();

};

#endif // WINDOWSTATEMANAGER_H
