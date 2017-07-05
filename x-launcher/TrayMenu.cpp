#include "stdafx.h"
#include "resource.h"
#include "TrayMenu.h"
#include "MainDlg.h"
#include "EditDlg.h"
#include "OutputDlg.h"

CTrayMenu::CTrayMenu(CWindow* pWnd, CConfig& config)
: m_pWnd(pWnd)
, m_config(config)
{

}

CTrayMenu::~CTrayMenu()
{

}

bool CTrayMenu::Init()
{
    if (m_grayIcon.IsNull())
        m_grayIcon.LoadFromResource(ModuleHelper::GetResourceInstance(), IDB_GRAY_ICON);

    if (m_greenIcon.IsNull())
        m_greenIcon.LoadFromResource(ModuleHelper::GetResourceInstance(), IDB_GREEN_ICON);

    if (m_redIcon.IsNull())
        m_redIcon.LoadFromResource(ModuleHelper::GetResourceInstance(), IDB_RED_ICON);


    return true;
}

bool CTrayMenu::Update()
{
    if (!m_menu.IsNull())
        m_menu.DestroyMenu();

    if (!m_menu.LoadMenu(IDR_MENU))
        return false;

    CTaskList& taskList = m_config.GetTaskList();
    if (taskList.empty())
        return true;

    CMenuHandle trayMenu(m_menu.GetSubMenu(0));

    UINT index = 0;
    for (CTask& task : taskList) {
        CMenuHandle subMenu;
        subMenu.CreatePopupMenu();
        InitSubMenu(subMenu, index);

        MENUITEMINFO mii = { 0 };
        mii.cbSize = sizeof(MENUITEMINFO);
        mii.fMask = MIIM_ID | MIIM_STRING | MIIM_SUBMENU | MIIM_BITMAP;
        mii.wID = index;
        mii.hSubMenu = (HMENU)subMenu;
        mii.dwTypeData = (LPTSTR)(LPCTSTR)task.name;
        mii.cch = task.name.GetLength();
        mii.hbmpItem = task.CheckIfRunning() ? m_greenIcon : m_grayIcon;
        trayMenu.InsertMenuItem(index, TRUE, &mii);

        // make that same space is reserved for the check mark and the bitmap
        // if not, menu item icon would have ugly appearance in xp
        // ref to: https://stackoverflow.com/questions/30325956/shell-extension-for-explorer-context-menu-icon-breaks-alignment-in-classic-wind
        MENUINFO mi = { 0 };
        mi.cbSize = sizeof(mi);
        mi.fMask = MIM_STYLE;
        trayMenu.GetMenuInfo(&mi);
        mi.dwStyle |= MNS_CHECKORBMP;
        trayMenu.SetMenuInfo(&mi);

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

    r = subMenu.InsertMenu((UINT)-1, MF_BYPOSITION | MF_SEPARATOR, (UINT_PTR)0, (LPCTSTR)NULL);
    ATLASSERT(r);

    UINT showMenuID = IDM_SUB_BEGIN + index * SUB_MENU_TOTAL_NUM + SUB_MENU_TYPE_SHOW;
    ATLASSERT(showMenuID < IDM_SUB_END);
    r = subMenu.InsertMenu((UINT)-1, flags, showMenuID, _T("Show"));
    ATLASSERT(r);

    return true;
}

void CTrayMenu::Show()
{
    if (!m_menu.IsMenu())
        return;

    ::SetForegroundWindow(m_pWnd->m_hWnd);

    CPoint pos;
    GetCursorPos(&pos);

    CMenuHandle trayMenu(m_menu.GetSubMenu(0));
    PrepareMenu(trayMenu);

    trayMenu.TrackPopupMenu(TPM_LEFTALIGN, pos.x, pos.y, m_pWnd->m_hWnd);

    m_pWnd->PostMessage(WM_NULL);
}

void CTrayMenu::PrepareMenu(CMenuHandle& menu)
{
    CTaskList& taskList = m_config.GetTaskList();

    int num = StartedTaskNum();
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

        // update task state color
        mii.fMask = MIIM_BITMAP;
        mii.hbmpItem = taskList[index].CheckIfRunning() ? m_greenIcon : m_grayIcon;
        r = menu.SetMenuItemInfo(index, TRUE, &mii);
        ATLASSERT(r);
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

void CTrayMenu::OnStartAll(UINT uNotifyCode, int nID, CWindow wndCtl)
{
    CMainDlg* pMainDlg = static_cast<CMainDlg*>(m_pWnd);
    ATLASSERT(pMainDlg != NULL);

    pMainDlg->StartAllTasks();
}

void CTrayMenu::OnStopAll(UINT uNotifyCode, int nID, CWindow wndCtl)
{
    CMainDlg* pMainDlg = static_cast<CMainDlg*>(m_pWnd);
    ATLASSERT(pMainDlg != NULL);

    pMainDlg->StopAllTasks();
}

void CTrayMenu::OnRunAtStartup(UINT uNotifyCode, int nID, CWindow wndCtl)
{
    CMainDlg* pMainDlg = static_cast<CMainDlg*>(m_pWnd);
    ATLASSERT(pMainDlg != NULL);

    bool runAtStartup = m_config.GetRunAtStartup();
    runAtStartup = !runAtStartup;

    bool result = false;
    if (runAtStartup)
        result = pMainDlg->SetRunAtStartup();
    else
        result = pMainDlg->RemoveRunAtStartup();

    if (result) {
        m_config.SetRunAtStartup(runAtStartup);
        m_config.Save();

        CMenuHandle trayMenu(m_menu.GetSubMenu(0));
        trayMenu.EnableMenuItem(IDM_TRAY_RUNATSTARTUP, MF_BYCOMMAND | (runAtStartup ? MF_CHECKED : MF_UNCHECKED));
    }
}

void CTrayMenu::OnExit(UINT uNotifyCode, int nID, CWindow wndCtl)
{
    CMainDlg* pMainDlg = static_cast<CMainDlg*>(m_pWnd);
    ATLASSERT(pMainDlg != NULL);

    if (StartedTaskNum() != 0) {
        if (m_pWnd->MessageBox(_T("Tasks are running, confirm to exit?"), _T("x-launcher"), MB_YESNO | MB_ICONQUESTION) == IDNO)
            return;
    }

    m_pWnd->PostMessage(WM_EXIT_FROM_MENU);
}

void CTrayMenu::OnSubMenuHandler(UINT uNotifyCode, int nID, CWindow wndCtl)
{
    UINT menuID = (UINT)nID;

    UINT index = (menuID - IDM_SUB_BEGIN) / SUB_MENU_TOTAL_NUM;

    CTaskList& taskList = m_config.GetTaskList();
    if (index >= taskList.size())
        return;

    CTask& task = taskList[index];
    int menuType = (menuID - IDM_SUB_BEGIN) % SUB_MENU_TOTAL_NUM;
    if (menuType == SUB_MENU_TYPE_START) {
        task.Launch();
    }
    else if (menuType == SUB_MENU_TYPE_STOP) {
        task.Terminate();
    }
    else if (menuType == SUB_MENU_TYPE_EDIT) {
        CEditDlg dlg(_T("Edit Task"), task);
        if (dlg.DoModal(m_pWnd->m_hWnd) != 0) {
            m_config.Save();

            Update();
        }
    }
    else if (menuType == SUB_MENU_TYPE_DELETE) {
        CString infoText;
        infoText.Format(_T("Are you sure you want to delete task [%s]?"), task.name);
        if (m_pWnd->MessageBox((LPCTSTR)infoText, _T("Delete Task"), MB_YESNO | MB_ICONWARNING) == IDYES) {
            for (CTaskList::iterator it = taskList.begin(); it != taskList.end(); it++) {
                if (&task == &*it) {
                    task.Terminate();
                    taskList.erase(it);
                    break;
                }
            }
            m_config.Save();

            Update();
        }
    }
    else if (menuType == SUB_MENU_TYPE_SHOW) {
        COutputDlg dlg(task);
        dlg.DoModal();
    }
}

void CTrayMenu::OnNewTask(UINT uNotifyCode, int nID, CWindow wndCtl)
{
    CTask newTask;
    CEditDlg dlg(_T("New Task"), newTask, true);
    if (dlg.DoModal(m_pWnd->m_hWnd) != 0) {
        m_config.GetTaskList().push_back(newTask);
        m_config.Save();

        Update();
    }
}

int CTrayMenu::StartedTaskNum()
{
    int num = 0;

    CTaskList& taskList = m_config.GetTaskList();
    for (CTaskList::iterator it = taskList.begin(); it != taskList.end(); it++) {
        CTask& task = *it;
        if (task.CheckIfRunning())
            num++;
    }

    return num;
}
