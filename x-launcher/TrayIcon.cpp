#include "stdafx.h"
#include "TrayIcon.h"


CTrayIcon::CTrayIcon()
: m_isInstalled(true)
{

}

CTrayIcon::~CTrayIcon()
{
    RemoveIcon();
}

bool CTrayIcon::InstallIcon(HWND hwnd, UINT id, HICON icon, UINT message, LPCTSTR toolTip)
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

void CTrayIcon::RemoveIcon()
{
    if (!m_isInstalled)
        return;

    m_nid.uFlags = 0;
    ::Shell_NotifyIcon(NIM_DELETE, &m_nid);

    m_isInstalled = false;
}
