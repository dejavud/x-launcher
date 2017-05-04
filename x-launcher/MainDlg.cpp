// MainDlg.cpp : implementation of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "MainDlg.h"
#include "EditDlg.h"

#define TRAY_ID 1000
#define REG_RUNATSTARTUP_KEY_NAME _T("x-launcher")

#define SUB_MENU_TOTAL_NUM 4
#define SUB_MENU_TYPE_START 0
#define SUB_MENU_TYPE_STOP 1
#define SUB_MENU_TYPE_EDIT 2
#define SUB_MENU_TYPE_DELETE 3

CMainDlg::CMainDlg()
{

}

CMainDlg::~CMainDlg()
{

}

BOOL CMainDlg::PreTranslateMessage(MSG* pMsg)
{
	return CWindow::IsDialogMessage(pMsg);
}

BOOL CMainDlg::OnIdle()
{
	UIUpdateChildWindows();
	return FALSE;
}

LRESULT CMainDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// center the dialog on the screen
	CenterWindow();

	// set icons
	HICON hIcon = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON));
	SetIcon(hIcon, TRUE);
	HICON hIconSmall = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
	SetIcon(hIconSmall, FALSE);

	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);

	UIAddChildWindowContainer(m_hWnd);

    InitData();
    InitMenu();

    m_trayIcon.InstallIcon(m_hWnd, TRAY_ID, hIconSmall, WM_TRAY_ICON, _T("x-launcher by ด๓นท"));

    if (m_config.GetAutoStart())
        StartAllTasks();

	return TRUE;
}

bool CMainDlg::InitData()
{
    m_config.Load();  // from json file
    m_config.ParseCmdline();  // from command line arguments
    m_config.SetRunAtStartup(IsRunAtStartup());  // from registry

    return true;
}

bool CMainDlg::InitMenu()
{
    if (!m_menu.IsNull())
        m_menu.DestroyMenu();

    if (!m_menu.LoadMenu(IDR_MENU))
        return false;

    if (m_config.GetTaskList().empty())
        return true;

    CMenuHandle trayMenu(m_menu.GetSubMenu(0));

    UINT index = 0;
    const CTaskList& taskList = m_config.GetTaskList();
    for (const CTask& task : taskList) {
        CMenuHandle subMenu;
        subMenu.CreatePopupMenu();
        InitSubMenu(index, subMenu);

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

bool CMainDlg::InitSubMenu(UINT index, CMenuHandle& subMenu)
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

LRESULT CMainDlg::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// unregister message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->RemoveMessageFilter(this);
	pLoop->RemoveIdleHandler(this);

	return 0;
}

LRESULT CMainDlg::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    CloseDialog(0);
    return 0;
}

void CMainDlg::CloseDialog(int nVal)
{
    m_trayIcon.RemoveIcon();

	DestroyWindow();
	::PostQuitMessage(nVal);
}

LRESULT CMainDlg::OnTrayIcon(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (LOWORD(lParam) == WM_RBUTTONUP)
    {
        if (m_menu.IsMenu()) {
            ::SetForegroundWindow(m_hWnd);

            CPoint pos;
            GetCursorPos(&pos);

            CMenuHandle oPopup(m_menu.GetSubMenu(0));
            PrepareMenu(oPopup);
            oPopup.TrackPopupMenu(TPM_LEFTALIGN, pos.x, pos.y, m_hWnd);
            PostMessage(WM_NULL);
        }
    }
    else if (LOWORD(lParam) == WM_LBUTTONDBLCLK)
    {
        
    }

    return 0;
}

void CMainDlg::PrepareMenu(HMENU hMenu)
{
    CMenuHandle trayMenu(hMenu);

    CTaskList& taskList = m_config.GetTaskList();

    int num = StartedTaskNum();
    trayMenu.EnableMenuItem(IDM_TRAY_STARTALL, MF_BYCOMMAND | ((size_t)num == taskList.size() ? MF_DISABLED : MF_ENABLED));
    trayMenu.EnableMenuItem(IDM_TRAY_STOPALL, MF_BYCOMMAND | (num == 0 ? MF_DISABLED : MF_ENABLED));
    trayMenu.CheckMenuItem(IDM_TRAY_RUNATSTARTUP, MF_BYCOMMAND | (m_config.GetRunAtStartup() ? MF_CHECKED : MF_UNCHECKED));

    for (UINT index = 0; index < taskList.size(); index++) {
        MENUITEMINFO mii = { 0 };
        mii.cbSize = sizeof(MENUITEMINFO);
        mii.fMask = MIIM_ID | MIIM_SUBMENU;
        mii.wID = index;
        BOOL r = trayMenu.GetMenuItemInfo(index, TRUE, &mii);
        ATLASSERT(r && mii.hSubMenu != NULL);

        CMenuHandle subMenu(mii.hSubMenu);
        PrepareSubMenu(subMenu, index);
    }
}

void CMainDlg::PrepareSubMenu(CMenuHandle& subMenu, UINT index)
{
    ATLASSERT(!subMenu.IsNull());

    CTaskList& taskList = m_config.GetTaskList();
    ATLASSERT(index < taskList.size());
    CTask& task = taskList[index];

    UINT startMenuID = IDM_SUB_BEGIN + index * SUB_MENU_TOTAL_NUM + SUB_MENU_TYPE_START;
    UINT stopMenuID = IDM_SUB_BEGIN + index * SUB_MENU_TOTAL_NUM + SUB_MENU_TYPE_STOP;
    bool isRunning = task.CheckIfRunning();
    subMenu.EnableMenuItem(startMenuID, MF_BYCOMMAND | (isRunning ? MF_DISABLED : MF_ENABLED));
    subMenu.EnableMenuItem(stopMenuID, MF_BYCOMMAND | (isRunning ? MF_ENABLED : MF_DISABLED));
}

void CMainDlg::OnStartAll(UINT uNotifyCode, int nID, CWindow wndCtl)
{
    StartAllTasks();
}

void CMainDlg::OnStopAll(UINT uNotifyCode, int nID, CWindow wndCtl)
{
    StopAllTasks();
}

void CMainDlg::OnRunAtStartup(UINT uNotifyCode, int nID, CWindow wndCtl)
{
    bool runAtStartup = m_config.GetRunAtStartup();
    runAtStartup = !runAtStartup;

    bool result = false;
    if (runAtStartup)
        result = SetRunAtStartup();
    else
        result = RemoveRunAtStartup();

    if (result) {
        m_config.SetRunAtStartup(runAtStartup);
        m_config.Save();
        UISetCheck(nID, runAtStartup);
    }
}

void CMainDlg::OnExit(UINT uNotifyCode, int nID, CWindow wndCtl)
{
    if (StartedTaskNum() != 0) {
        if (MessageBox(_T("Tasks are running, confirm to exit?"), _T("x-launcher"), MB_YESNO | MB_ICONQUESTION) == IDYES) {
            StopAllTasks();
        }
        else {
            return;
        }
    }

    CloseDialog(0);
}

bool CMainDlg::StartAllTasks()
{
    CTaskList& taskList = m_config.GetTaskList();
    if (taskList.empty())
        return false;

    for (CTaskList::iterator it = taskList.begin(); it != taskList.end(); it++) {
        CTask& task = *it;
        task.Launch();
    }

    return true;
}

void CMainDlg::StopAllTasks()
{
    CTaskList& taskList = m_config.GetTaskList();
    for (CTaskList::iterator it = taskList.begin(); it != taskList.end(); it++) {
        CTask& task = *it;
        task.Terminate();
    }
}

int CMainDlg::StartedTaskNum()
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

bool CMainDlg::SetRunAtStartup()
{
    HKEY hKey;
    if (::RegOpenKeyEx(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Run"), 0, KEY_ALL_ACCESS, &hKey) != ERROR_SUCCESS)
        return false;

    TCHAR appPath[MAX_PATH];
    ::GetModuleFileName(NULL, appPath, MAX_PATH);
    CString value = appPath;
    value += " --autostart";

    LONG r = ::RegSetValueEx(hKey, REG_RUNATSTARTUP_KEY_NAME, NULL, REG_SZ, (LPBYTE)(LPCTSTR)value, (_tcslen(value) + 1) * sizeof(TCHAR));

    ::RegCloseKey(hKey);

    return (r == ERROR_SUCCESS);
}

bool CMainDlg::RemoveRunAtStartup()
{
    HKEY hKey;
    if (::RegOpenKeyEx(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Run"), 0, KEY_SET_VALUE, &hKey) != ERROR_SUCCESS)
        return false;

    LONG r = ::RegDeleteValue(hKey, REG_RUNATSTARTUP_KEY_NAME);

    ::RegCloseKey(hKey);

    return (r == ERROR_SUCCESS);
}

bool CMainDlg::IsRunAtStartup()
{
    HKEY hKey;
    if (::RegOpenKeyEx(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Run"), 0, KEY_QUERY_VALUE, &hKey) != ERROR_SUCCESS)
        return false;

    LONG r = ::RegQueryValueEx(hKey, REG_RUNATSTARTUP_KEY_NAME, NULL, NULL, NULL, NULL);

    ::RegCloseKey(hKey);

    return r == ERROR_SUCCESS;
}

void CMainDlg::OnSubMenuHandler(UINT uNotifyCode, int nID, CWindow wndCtl)
{
    UINT index = (nID - IDM_SUB_BEGIN) / SUB_MENU_TOTAL_NUM;

    CTaskList& taskList = m_config.GetTaskList();
    if (index >= taskList.size())
        return;
    
    CTask& task = taskList[index];
    int menuType = (nID - IDM_SUB_BEGIN) % SUB_MENU_TOTAL_NUM;
    if (menuType == SUB_MENU_TYPE_START) {
        task.Launch();
    }
    else if (menuType == SUB_MENU_TYPE_STOP) {
        task.Terminate();
    }
    else if (menuType == SUB_MENU_TYPE_EDIT) {
        CEditDlg dlg(_T("Edit Task"), task);
        if (dlg.DoModal(m_hWnd) != 0) {
            m_config.Save();

            InitMenu(); // recreate tray menu
        }
    }
    else if (menuType == SUB_MENU_TYPE_DELETE) {
        CString infoText;
        infoText.Format(_T("Are you sure you want to delete task [%s]?"), task.name);
        if (MessageBox((LPCTSTR)infoText, _T("Delete Task"), MB_YESNO | MB_ICONWARNING) == IDYES) {
            for (CTaskList::iterator it = taskList.begin(); it != taskList.end(); it++) {
                if (&task == &*it) {
                    task.Terminate();
                    taskList.erase(it);
                    break;
                }
            }
            m_config.Save();

            InitMenu(); // recreate tray menu
        }
    }
}

void CMainDlg::OnNewTask(UINT uNotifyCode, int nID, CWindow wndCtl)
{
    CTask newTask;
    CEditDlg dlg(_T("New Task"), newTask, true);
    if (dlg.DoModal(m_hWnd) != 0) {
        m_config.GetTaskList().push_back(newTask);
        m_config.Save();

        InitMenu(); // recreate tray menu
    }
}
