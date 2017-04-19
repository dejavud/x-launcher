#pragma once

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
    CTrayIcon();
    ~CTrayIcon();

public:
    bool InstallIcon(HWND hwnd, UINT id, HICON icon, UINT message, LPCTSTR toolTip);
    void RemoveIcon();

private:
    bool m_isInstalled;
    CNotifyIconData m_nid;
};

