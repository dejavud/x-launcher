#pragma once

#include "TrayMenu.h"
#include "CommonDefs.h"

enum CTrayBallonStyle
{
    CBS_NONE = 0,
    CBS_INFO,
    CBS_WARNING,
    CBS_ERROR,
    CBS_USER,
};

class CNotifyIconData : public NOTIFYICONDATA
{
public:
    CNotifyIconData()
    {
        memset(this, 0, sizeof(NOTIFYICONDATA));
        cbSize = sizeof(NOTIFYICONDATA);
    }
};

class CTrayIcon
{
public:
    CTrayIcon(CWindow* pWnd, CConfig& config);
    ~CTrayIcon();

public:
    BEGIN_MSG_MAP_EX(CTrayIcon)
        MESSAGE_HANDLER_EX(WM_TRAY_ICON, OnTrayIcon)
        CHAIN_MSG_MAP_MEMBER(m_trayMenu)
    END_MSG_MAP()

    LRESULT OnTrayIcon(UINT uMsg, WPARAM wParam, LPARAM lParam);

    bool Install();
    void Remove();

    bool ShowBallon(const CString& info, const CString& title, CTrayBallonStyle style);

protected:
    bool InstallTrayIcon(HWND hwnd, UINT id, HICON icon, UINT message, LPCTSTR toolTip);
    void RemoveTrayIcon();

private:
    bool m_isInstalled;
    CNotifyIconData m_nid;

    CWindow* m_pWnd;
    CTrayMenu m_trayMenu;
};

