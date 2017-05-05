// MainDlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Config.h"
#include "TrayIcon.h"
#include "TrayMenu.h"
#include "CommonDefs.h"

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
        MESSAGE_HANDLER_EX(WM_TRAY_ICON, OnTrayIcon)
        CHAIN_MSG_MAP_MEMBER(m_trayMenu)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnTrayIcon(UINT uMsg, WPARAM wParam, LPARAM lParam);

    bool StartAllTasks();
    void StopAllTasks();

    bool SetRunAtStartup();
    bool RemoveRunAtStartup();
    bool IsRunAtStartup();

private:
    bool InitData();
	void CloseDialog(int nVal);

private:
    CConfig m_config;
    CTrayIcon m_trayIcon;
    CTrayMenu m_trayMenu;
};
