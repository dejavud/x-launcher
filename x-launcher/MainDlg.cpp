// MainDlg.cpp : implementation of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "MainDlg.h"
#include "CommonDefs.h"

#define REG_RUNATSTARTUP_KEY_NAME _T("x-launcher")

CMainDlg::CMainDlg()
: m_trayIcon(this, m_config)
{

}

CMainDlg::~CMainDlg()
{

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

    InitData();

    if (!m_trayIcon.Install()) {
        CloseDialog(0);
        return FALSE;
    }

    SetTimer(TI_TASKS_PATROL, 250);  // every 250ms

	return TRUE;
}

bool CMainDlg::InitData()
{
    m_config.Load();  // from json file
    m_config.ParseCmdline();  // from command line arguments
    m_config.SetRunAtStartup(IsRunAtStartup());  // from registry

    AutoStartTasks();

    return true;
}

LRESULT CMainDlg::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	return 0;
}

LRESULT CMainDlg::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    CloseDialog(0);
    return 0;
}

void CMainDlg::CloseDialog(int nVal)
{
    KillTimer(TI_TASKS_PATROL);

    m_trayIcon.Remove();

	DestroyWindow();
	::PostQuitMessage(nVal);
}

LRESULT CMainDlg::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (wParam)
    {
    case TI_TASKS_PATROL:
        OnTasksPatrol();
        break;
    default:
        break;
    }

    return 0;
}

LRESULT CMainDlg::OnExitFromMenu(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    StopAllTasks();

    int val = (int)wParam;
    CloseDialog(val);
    return 0;
}

bool CMainDlg::StartAllTasks()
{
    CTaskList& taskList = m_config.GetTaskList();
    if (taskList.empty())
        return false;

    for (CTask& task : taskList)
        task.Launch();

    return true;
}

void CMainDlg::StopAllTasks()
{
    CTaskList& taskList = m_config.GetTaskList();
    for (CTask& task : taskList)
        task.Terminate();
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

void CMainDlg::AutoStartTasks()
{
    CTaskList& taskList = m_config.GetTaskList();

    for (CTask& task : taskList) {
        if (task.autostart)
            task.Launch();
    }
}

void CMainDlg::OnTasksPatrol()
{
    CTaskList& taskList = m_config.GetTaskList();
    for (CTask& task : taskList) {
        if (task.IsLaunched() && !task.CheckIfRunning()) {
            task.Terminate();

            if (task.autorestart) 
                task.Launch();
        }

        if (task.CheckIfRunning())
            task.ReadOutput();
    }
}
