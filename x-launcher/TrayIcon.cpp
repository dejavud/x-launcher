#include "stdafx.h"
#include "resource.h"
#include "TrayIcon.h"

#define TRAY_ID 1000
#define TRAY_TOOLTIP _T("x-launcher by ด๓นท")

CTrayIcon::CTrayIcon(CWindow* pWnd, CConfig& config)
: m_isInstalled(true)
, m_pWnd(pWnd)
, m_trayMenu(pWnd, config)
{

}

CTrayIcon::~CTrayIcon()
{
    Remove();
}

bool CTrayIcon::Install()
{
    ATLASSERT(m_pWnd != NULL && m_pWnd->m_hWnd != NULL);

    m_trayMenu.Init();
    m_trayMenu.Update();

    HICON hIconSmall = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
    return InstallTrayIcon(m_pWnd->m_hWnd, TRAY_ID, hIconSmall, WM_TRAY_ICON, TRAY_TOOLTIP);
}

void CTrayIcon::Remove()
{
    RemoveTrayIcon();
}

LRESULT CTrayIcon::OnTrayIcon(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (LOWORD(lParam) == WM_RBUTTONUP)
    {
        m_trayMenu.Show();
    }
    else if (LOWORD(lParam) == WM_LBUTTONDBLCLK)
    {

    }

    return 0;
}

bool CTrayIcon::InstallTrayIcon(HWND hwnd, UINT id, HICON icon, UINT message, LPCTSTR toolTip)
{
    ATLASSERT(hwnd != NULL);

    m_nid.hWnd = hwnd;
    m_nid.uID = id;
    m_nid.hIcon = icon;
    m_nid.uCallbackMessage = message;
    _tcscpy(m_nid.szTip, toolTip);
    m_nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;

    m_isInstalled = ::Shell_NotifyIcon(NIM_ADD, &m_nid) ? true : false;
    return m_isInstalled;
}

void CTrayIcon::RemoveTrayIcon()
{
    if (!m_isInstalled)
        return;

    m_nid.uFlags = 0;
    ::Shell_NotifyIcon(NIM_DELETE, &m_nid);

    m_isInstalled = false;
}
