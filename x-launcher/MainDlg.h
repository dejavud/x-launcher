// MainDlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Config.h"
#include "TrayIcon.h"
#include "TrayMenu.h"

class CMainDlg : 
    public CDialogImpl<CMainDlg>
{
public:
	enum { IDD = IDD_MAINDLG };

    CMainDlg();
    ~CMainDlg();

    BEGIN_MSG_MAP_EX(CMainDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
        MESSAGE_HANDLER_EX(WM_CLOSE, OnClose)
        MESSAGE_HANDLER_EX(WM_TIMER, OnTimer)
        MESSAGE_HANDLER_EX(WM_EXIT_FROM_MENU, OnExitFromMenu)
        CHAIN_MSG_MAP_MEMBER(m_trayIcon)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnExitFromMenu(UINT uMsg, WPARAM wParam, LPARAM lParam);

    bool StartAllTasks();
    void StopAllTasks();

    bool SetRunAtStartup();
    bool RemoveRunAtStartup();
    bool IsRunAtStartup();

private:
    bool InitData();
	void CloseDialog(int nVal);

    void OnProcessTasksOutput();

private:
    CConfig m_config;
    CTrayIcon m_trayIcon;
};
