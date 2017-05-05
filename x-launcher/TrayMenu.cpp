#include "stdafx.h"
#include "TrayMenu.h"
#include "resource.h"

#define SUB_MENU_TOTAL_NUM 4
#define SUB_MENU_TYPE_START 0
#define SUB_MENU_TYPE_STOP 1
#define SUB_MENU_TYPE_EDIT 2
#define SUB_MENU_TYPE_DELETE 3

CTrayMenu::CTrayMenu(CConfig& config)
: m_config(config)
{

}

CTrayMenu::~CTrayMenu()
{

}

bool CTrayMenu::Create()
{
    if (!m_menu.IsNull())
        m_menu.DestroyMenu();

    if (!m_menu.LoadMenu(IDR_MENU))
        return false;

    const CTaskList& taskList = m_config.GetTaskList();
    if (taskList.empty())
        return true;

    CMenuHandle trayMenu(m_menu.GetSubMenu(0));

    UINT index = 0;
    for (const CTask& task : taskList) {
        CMenuHandle subMenu;
        subMenu.CreatePopupMenu();
        InitSubMenu(subMenu, index);

        MENUITEMINFO mii = { 0 };
        mii.cbSize = sizeof(MENUITEMINFO);
        mii.fMask = MIIM_ID | MIIM_STRING | MIIM_SUBMENU;
        mii.wID = index;
        mii.hSubMenu = (HMENU)subMenu;
        mii.dwTypeData = (LPTSTR)(LPCTSTR)task.name;
        mii.cch = task.name.GetLength();
        trayMenu.InsertMenuItem(index, TRUE, &mii);

        ++index;
    }

    if (!trayMenu.InsertMenu(index, MF_BYPOSITION | MF_SEPARATOR))
        return false;

    return true;
}

bool CTrayMenu::InitSubMenu(CMenuHandle& subMenu, UINT index)
{
    ATLASSERT(!subMenu.IsNull());

    UINT flags = MF_BYPOSITION | MF_POPUP | MF_STRING;

    UINT startMenuID = IDM_SUB_BEGIN + index * SUB_MENU_TOTAL_NUM + SUB_MENU_TYPE_START;
    ATLASSERT(startMenuID < IDM_SUB_END);
    BOOL r = subMenu.InsertMenu((UINT)-1, flags, startMenuID, _T("Start"));
    ATLASSERT(r);

    UINT stopMenuID = IDM_SUB_BEGIN + index * SUB_MENU_TOTAL_NUM + SUB_MENU_TYPE_STOP;
    ATLASSERT(stopMenuID < IDM_SUB_END);
    r = subMenu.InsertMenu((UINT)-1, flags, stopMenuID, _T("Stop"));
    ATLASSERT(r);

    UINT editMenuID = IDM_SUB_BEGIN + index * SUB_MENU_TOTAL_NUM + SUB_MENU_TYPE_EDIT;
    ATLASSERT(editMenuID < IDM_SUB_END);
    r = subMenu.InsertMenu((UINT)-1, flags, editMenuID, _T("Edit"));
    ATLASSERT(r);

    UINT deleteMenuID = IDM_SUB_BEGIN + index * SUB_MENU_TOTAL_NUM + SUB_MENU_TYPE_DELETE;
    ATLASSERT(deleteMenuID < IDM_SUB_END);
    r = subMenu.InsertMenu((UINT)-1, flags, deleteMenuID, _T("Delete"));
    ATLASSERT(r);

    return true;
}

void CTrayMenu::Show(HWND hWnd)
{
    if (!m_menu.IsMenu())
        return;

    ::SetForegroundWindow(hWnd);

    CPoint pos;
    GetCursorPos(&pos);

    CMenuHandle trayMenu(m_menu.GetSubMenu(0));
    PrepareMenu(trayMenu);

    trayMenu.TrackPopupMenu(TPM_LEFTALIGN, pos.x, pos.y, hWnd);

    ::PostMessage(hWnd, WM_NULL, 0, 0);
}

void CTrayMenu::PrepareMenu(CMenuHandle& menu)
{
    CTaskList& taskList = m_config.GetTaskList();

    int num = StartedTaskNum(taskList);
    menu.EnableMenuItem(IDM_TRAY_STARTALL, MF_BYCOMMAND | ((size_t)num == taskList.size() ? MF_DISABLED : MF_ENABLED));
    menu.EnableMenuItem(IDM_TRAY_STOPALL, MF_BYCOMMAND | (num == 0 ? MF_DISABLED : MF_ENABLED));
    menu.CheckMenuItem(IDM_TRAY_RUNATSTARTUP, MF_BYCOMMAND | (m_config.GetRunAtStartup() ? MF_CHECKED : MF_UNCHECKED));

    for (UINT index = 0; index < taskList.size(); index++) {
        MENUITEMINFO mii = { 0 };
        mii.cbSize = sizeof(MENUITEMINFO);
        mii.fMask = MIIM_ID | MIIM_SUBMENU;
        mii.wID = index;
        BOOL r = menu.GetMenuItemInfo(index, TRUE, &mii);
        ATLASSERT(r && mii.hSubMenu != NULL);

        CMenuHandle subMenu(mii.hSubMenu);
        PrepareSubMenu(subMenu, index, taskList[index]);
    }
}

void CTrayMenu::PrepareSubMenu(CMenuHandle& subMenu, UINT index, CTask& task)
{
    ATLASSERT(!subMenu.IsNull());

    UINT startMenuID = IDM_SUB_BEGIN + index * SUB_MENU_TOTAL_NUM + SUB_MENU_TYPE_START;
    UINT stopMenuID = IDM_SUB_BEGIN + index * SUB_MENU_TOTAL_NUM + SUB_MENU_TYPE_STOP;
    bool isRunning = task.CheckIfRunning();
    subMenu.EnableMenuItem(startMenuID, MF_BYCOMMAND | (isRunning ? MF_DISABLED : MF_ENABLED));
    subMenu.EnableMenuItem(stopMenuID, MF_BYCOMMAND | (isRunning ? MF_ENABLED : MF_DISABLED));
}
