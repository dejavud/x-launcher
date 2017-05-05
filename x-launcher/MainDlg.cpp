// MainDlg.cpp : implementation of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "MainDlg.h"


CMainDlg::CMainDlg()
: m_trayMenu(this, m_config)
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
    m_trayMenu.Create();

    m_trayIcon.InstallIcon(m_hWnd, TRAY_ID, hIconSmall, WM_TRAY_ICON, TRAY_TOOLTIP);

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
        m_trayMenu.Show();
    }
    else if (LOWORD(lParam) == WM_LBUTTONDBLCLK)
    {
        
    }

    return 0;
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
