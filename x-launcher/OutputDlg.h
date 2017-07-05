#pragma once

#include "Task.h"

class COutputDlg :
    public CDialogImpl<COutputDlg>,
    public CWinDataExchange<COutputDlg>
{
public:
    enum { IDD = IDD_OUTPUTDLG };

    COutputDlg(CTask& task);
    ~COutputDlg();

    BEGIN_MSG_MAP_EX(COutputDlg)
        MESSAGE_HANDLER_EX(WM_INITDIALOG, OnInitDialog)
        MESSAGE_HANDLER_EX(WM_CLOSE, OnClose)
    END_MSG_MAP()

    BEGIN_DDX_MAP(COutputDlg)
    END_DDX_MAP()

    LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
    void InitUI();
    void CloseDialog(int val);

    static void ReadOutputCallback(const CString& str, UINT_PTR param);
    void OnRefresh(const CString& str);

private:
    CTask& m_task;
    CRichEditCtrl m_outputEdit;
};
