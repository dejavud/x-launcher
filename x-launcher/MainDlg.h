// MainDlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Config.h"
#include "TrayIcon.h"
#include "TrayMenu.h"

class CMainDlg : 
    public CDialogImpl<CMainDlg>, 
    public CUpdateUI<CMainDlg>,
	public CMessageFilter, 
    public CIdleHandler
{
public:
	enum { IDD = IDD_MAINDLG };

    enum {
        WM_TRAY_ICON = WM_USER + 1000,
    };

    CMainDlg();
    ~CMainDlg();

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnIdle();

	BEGIN_UPDATE_UI_MAP(CMainDlg)
	END_UPDATE_UI_MAP()

    BEGIN_MSG_MAP_EX(CMainDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
        MESSAGE_HANDLER_EX(WM_CLOSE, OnClose)
        MESSAGE_HANDLER_EX(WM_TRAY_ICON, OnTrayIcon)
        COMMAND_ID_HANDLER_EX(IDM_TRAY_STARTALL, OnStartAll)
        COMMAND_ID_HANDLER_EX(IDM_TRAY_STOPALL, OnStopAll)
        COMMAND_ID_HANDLER_EX(IDM_TRAY_RUNATSTARTUP, OnRunAtStartup)
        COMMAND_ID_HANDLER_EX(IDM_TRAY_EXIT, OnExit)
        COMMAND_RANGE_HANDLER_EX(IDM_SUB_BEGIN, IDM_SUB_END, OnSubMenuHandler)
        COMMAND_ID_HANDLER_EX(IDM_NEW_TASK, OnNewTask)
        CHAIN_MSG_MAP(CUpdateUI<CMainDlg>)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnTrayIcon(UINT uMsg, WPARAM wParam, LPARAM lParam);
    void OnStartAll(UINT uNotifyCode, int nID, CWindow wndCtl);
    void OnStopAll(UINT uNotifyCode, int nID, CWindow wndCtl);
    void OnRunAtStartup(UINT uNotifyCode, int nID, CWindow wndCtl);
    void OnExit(UINT uNotifyCode, int nID, CWindow wndCtl);
    void OnSubMenuHandler(UINT uNotifyCode, int nID, CWindow wndCtl);
    void OnNewTask(UINT uNotifyCode, int nID, CWindow wndCtl);

private:
    bool InitData();
	void CloseDialog(int nVal);
        
    bool StartAllTasks();
    void StopAllTasks();

    bool SetRunAtStartup();
    bool RemoveRunAtStartup();
    bool IsRunAtStartup();

private:
    CConfig m_config;
    CTrayIcon m_trayIcon;
    CTrayMenu m_trayMenu;
};
