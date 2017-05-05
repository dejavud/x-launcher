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
: m_trayMenu(m_config)
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
    m_trayMenu.Create();

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
        m_trayMenu.Show(m_hWnd);
    }
    else if (LOWORD(lParam) == WM_LBUTTONDBLCLK)
    {
        
    }

    return 0;
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
    if (StartedTaskNum(m_config.GetTaskList()) != 0) {
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

            m_trayMenu.Create(); // recreate tray menu
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

            m_trayMenu.Create(); // recreate tray menu
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

        m_trayMenu.Create(); // recreate tray menu
    }
}
