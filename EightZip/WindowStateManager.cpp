#include "stdwx.h"
#include "WindowStateManager.h"

#include "EightZipConfig.h"

WindowStateManager::WindowStateManager(wxTopLevelWindowBase *pWindow,
    ConfigIndex indexLocationX,
    ConfigIndex indexLocationY,
    ConfigIndex indexWidth,
    ConfigIndex indexHeight,
    ConfigIndex indexIsMaximized)
{
    Create(pWindow,
        indexLocationX,
        indexLocationY,
        indexWidth,
        indexHeight,
        indexIsMaximized);
}

WindowStateManager::~WindowStateManager()
{
    Destroy();
}

void WindowStateManager::Create(wxTopLevelWindowBase *pWindow,
    ConfigIndex indexLocationX,
    ConfigIndex indexLocationY,
    ConfigIndex indexWidth,
    ConfigIndex indexHeight,
    ConfigIndex indexIsMaximized)
{
    assert(pWindow);
    m_pWindow = pWindow;
    m_indexLocationX = indexLocationX;
    m_indexLocationY = indexLocationY;
    m_indexWidth = indexWidth;
    m_indexHeight = indexHeight;
    m_indexIsMaximized = indexIsMaximized;
    m_pWindow->Bind(wxEVT_MOVE, &WindowStateManager::_OnMove, this);
    m_pWindow->Bind(wxEVT_SIZE, &WindowStateManager::_OnSize, this);
    LoadState();
}

void WindowStateManager::LoadState()
{
    assert(m_pWindow);
    auto &config = EightZipConfig::GetInstance();
    m_pWindow->SetClientSize(
        config.GetInteger(m_indexWidth),
        config.GetInteger(m_indexHeight));
    int nX;
    int nY;
    if (config.GetInteger(m_indexLocationX, &nX)
        && config.GetInteger(m_indexLocationY, &nY))
    {
        m_pWindow->SetPosition(wxPoint(nX, nY));
    }
    else
    {
        m_pWindow->Center();
    }
    m_pWindow->Maximize(config.GetBoolean(m_indexIsMaximized));
}

void WindowStateManager::SaveState() const
{
    auto &config = EightZipConfig::GetInstance();
    config.Set(m_indexLocationX, m_lastPosition.x);
    config.Set(m_indexLocationY, m_lastPosition.y);
    config.Set(m_indexWidth, m_lastSize.GetWidth());
    config.Set(m_indexHeight, m_lastSize.GetHeight());
    config.Set(m_indexIsMaximized, m_isLastMaxmized);
}

void WindowStateManager::Destroy()
{
    if (m_pWindow)
    {
        m_pWindow->Unbind(wxEVT_MOVE, &WindowStateManager::_OnMove, this);
        m_pWindow->Unbind(wxEVT_SIZE, &WindowStateManager::_OnSize, this);
        m_pWindow = nullptr;
        SaveState();
    }
}

void WindowStateManager::_OnMove(wxMoveEvent& event)
{
    _UpdateLastLayout();
    event.Skip();
}

void WindowStateManager::_OnSize(wxSizeEvent& event)
{
    _UpdateLastLayout();
    event.Skip();
}

void WindowStateManager::_UpdateLastLayout()
{
    if (!m_pWindow->IsIconized())
    {
        m_isLastMaxmized = m_pWindow->IsMaximized();
        if (!m_isLastMaxmized)
        {
            m_lastPosition = m_pWindow->GetPosition();
            m_lastSize = m_pWindow->GetClientSize();
        }
    }
}
