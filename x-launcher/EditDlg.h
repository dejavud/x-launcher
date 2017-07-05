#pragma once

#include "Task.h"

class CEditDlg :
    public CDialogImpl<CEditDlg>,
    public CWinDataExchange<CEditDlg>
{
public:
    enum { IDD = IDD_EDITDLG };

    CEditDlg(const CString& title, CTask& task, bool isNew = false);
    ~CEditDlg();

    BEGIN_MSG_MAP_EX(CEditDlg)
        MESSAGE_HANDLER_EX(WM_INITDIALOG, OnInitDialog)
        MESSAGE_HANDLER_EX(WM_CLOSE, OnClose)
        COMMAND_ID_HANDLER_EX(IDC_BTN_SAVE, OnSave)
        COMMAND_ID_HANDLER_EX(IDC_BTN_CANCEL, OnCancel)
        COMMAND_ID_HANDLER_EX(IDC_BTN_PATH, OnPathBrowse)
        COMMAND_ID_HANDLER_EX(IDC_BTN_DIR, OnDirBrowse)
    END_MSG_MAP()

    BEGIN_DDX_MAP(CEditDlg)
        DDX_TEXT(IDC_EDIT_NAME, m_nameText)
        DDX_TEXT(IDC_EDIT_PATH, m_pathText)
        DDX_TEXT(IDC_EDIT_DIR, m_dirText)
        DDX_TEXT(IDC_EDIT_ARGS, m_argsText)
        DDX_CHECK(IDC_CHECK_AUTO_START, m_autostartCheck)
        DDX_CHECK(IDC_CHECK_AUTO_RESTART, m_autorestartCheck)
    END_DDX_MAP()

    LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam);
    void OnSave(UINT uNotifyCode, int nID, CWindow wndCtl);
    void OnCancel(UINT uNotifyCode, int nID, CWindow wndCtl);
    void OnPathBrowse(UINT uNotifyCode, int nID, CWindow wndCtl);
    void OnDirBrowse(UINT uNotifyCode, int nID, CWindow wndCtl);

private:
    void InitUI();
    void InitNewTask(CTask& task);
    void CloseDialog(int val);

private:
    CString m_title;
    CTask& m_task;
    bool m_isNew;

    CString m_nameText;
    CString m_pathText;
    CString m_dirText;
    CString m_argsText;
    bool m_autostartCheck;
    bool m_autorestartCheck;
};
