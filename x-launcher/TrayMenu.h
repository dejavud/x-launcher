#pragma once

#include "Config.h"
#include "CommonDefs.h"

class CTrayMenu
{
public:
    CTrayMenu(CWindow* pWnd, CConfig& config);
    ~CTrayMenu();

public:
    BEGIN_MSG_MAP_EX(CTrayMenu)
        COMMAND_ID_HANDLER_EX(IDM_TRAY_STARTALL, OnStartAll)
        COMMAND_ID_HANDLER_EX(IDM_TRAY_STOPALL, OnStopAll)
        COMMAND_ID_HANDLER_EX(IDM_TRAY_RUNATSTARTUP, OnRunAtStartup)
        COMMAND_ID_HANDLER_EX(IDM_TRAY_EXIT, OnExit)
        COMMAND_RANGE_HANDLER_EX(IDM_SUB_BEGIN, IDM_SUB_END, OnSubMenuHandler)
        COMMAND_ID_HANDLER_EX(IDM_NEW_TASK, OnNewTask)
    END_MSG_MAP()

    void OnStartAll(UINT uNotifyCode, int nID, CWindow wndCtl);
    void OnStopAll(UINT uNotifyCode, int nID, CWindow wndCtl);
    void OnRunAtStartup(UINT uNotifyCode, int nID, CWindow wndCtl);
    void OnExit(UINT uNotifyCode, int nID, CWindow wndCtl);
    void OnSubMenuHandler(UINT uNotifyCode, int nID, CWindow wndCtl);
    void OnNewTask(UINT uNotifyCode, int nID, CWindow wndCtl);

    bool Init();
    bool Update();
    void Show();

protected:
    bool InitSubMenu(CMenuHandle& subMenu, UINT index);
    void PrepareMenu(CMenuHandle& menu);
    void PrepareSubMenu(CMenuHandle& subMenu, UINT index, CTask& task);

private:
    CWindow* m_pWnd;
    CConfig& m_config;
    CMenu m_menu;

    CImage m_grayIcon, m_redIcon, m_greenIcon;
};
