// MainDlg.cpp : implementation of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "MainDlg.h"

#define TRAY_ID 1000
#define REG_RUNATSTARTUP_KEY_NAME _T("x-launcher")

CMainDlg::CMainDlg()
: m_isStarted(FALSE)
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

    m_config.Load();  // from json file
    m_config.ParseCmdline();  // from command line arguments
    m_config.SetRunAtStartup(CheckRunAtStartup());  // from registry

    m_trayIcon.InstallIcon(m_hWnd, TRAY_ID, hIconSmall, WM_TRAY_ICON, _T("x-launcher by ด๓นท"));

    if (m_config.GetAutoStart())
        StartAllTasks();

	return TRUE;
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
        CMenu oMenu;
        if (oMenu.LoadMenu(IDR_MENU)) {
            ::SetForegroundWindow(m_hWnd);

            CPoint pos;
            GetCursorPos(&pos);

            CMenuHandle oPopup(oMenu.GetSubMenu(0));
            PrepareMenu(oPopup);
            oPopup.TrackPopupMenu(TPM_LEFTALIGN, pos.x, pos.y, m_hWnd);
            PostMessage(WM_NULL);

            oMenu.DestroyMenu();
        }
    }
    else if (LOWORD(lParam) == WM_LBUTTONDBLCLK)
    {
        
    }

    return 0;
}

void CMainDlg::PrepareMenu(HMENU hMenu)
{
    UIEnable(ID_TRAY_STARTALL, !m_isStarted);
    UIEnable(ID_TRAY_STOPALL, m_isStarted);
    UISetCheck(ID_TRAY_RUNATSTARTUP, m_config.GetRunAtStartup());
}

void CMainDlg::OnStartAll(UINT uNotifyCode, int nID, CWindow wndCtl)
{
    m_isStarted = (StartAllTasks() == true);
}

void CMainDlg::OnStopAll(UINT uNotifyCode, int nID, CWindow wndCtl)
{
    StopAllTasks();
    m_isStarted = FALSE;
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
    if (m_isStarted) {
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

    m_isStarted = TRUE;
    return true;
}

void CMainDlg::StopAllTasks()
{
    if (!m_isStarted)
        return;

    CTaskList& taskList = m_config.GetTaskList();
    for (CTaskList::iterator it = taskList.begin(); it != taskList.end(); it++) {
        CTask& task = *it;
        task.Term();
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

bool CMainDlg::CheckRunAtStartup()
{
    HKEY hKey;
    if (::RegOpenKeyEx(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Run"), 0, KEY_QUERY_VALUE, &hKey) != ERROR_SUCCESS)
        return false;

    LONG r = ::RegQueryValueEx(hKey, REG_RUNATSTARTUP_KEY_NAME, NULL, NULL, NULL, NULL);

    ::RegCloseKey(hKey);

    return r == ERROR_SUCCESS;
}
