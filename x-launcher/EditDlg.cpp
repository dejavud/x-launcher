#include "stdafx.h"
#include "resource.h"
#include "EditDlg.h"


CEditDlg::CEditDlg(const CString& title, CTask& task, bool isNew)
: m_title(title)
, m_task(task)
, m_isNew(isNew)
, m_autostartCheck(false)
, m_autorestartCheck(false)
{

}

CEditDlg::~CEditDlg()
{

}

LRESULT CEditDlg::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    CenterWindow();

    HICON hIcon = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON));
    SetIcon(hIcon, TRUE);
    HICON hIconSmall = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
    SetIcon(hIconSmall, FALSE);

    InitUI();

    return TRUE;
}

LRESULT CEditDlg::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    CloseDialog(0);
    return 0;
}

void CEditDlg::InitUI()
{
    ::SetWindowText(m_hWnd, (LPCTSTR)m_title);

    if (m_isNew)
        InitNewTask(m_task);

    m_nameText = m_task.name;
    m_pathText = m_task.path;
    m_dirText = m_task.dir;
    m_argsText = m_task.args;
    m_autostartCheck = m_task.autostart;
    m_autorestartCheck = m_task.autorestart;

    DoDataExchange(FALSE);
}

void CEditDlg::InitNewTask(CTask& task)
{
    task.name = _T("Unnamed");
}

void CEditDlg::CloseDialog(int val)
{
    EndDialog(val);
}

void CEditDlg::OnSave(UINT uNotifyCode, int nID, CWindow wndCtl)
{
    DoDataExchange(TRUE);

    if (m_nameText.IsEmpty() || m_pathText.IsEmpty()) {
        MessageBox(_T("Name or Path cannot be empty."), _T("Warning"), MB_OK | MB_ICONWARNING);
        return;
    }

    if (m_dirText.IsEmpty())
        m_dirText = m_pathText.Left(m_pathText.ReverseFind(_T('\\')));

    m_task.name = m_nameText;
    m_task.path = m_pathText;
    m_task.dir = m_dirText;
    m_task.args = m_argsText;
    m_task.autostart = m_autostartCheck;
    m_task.autorestart = m_autorestartCheck;

    CloseDialog(1);
}

void CEditDlg::OnCancel(UINT uNotifyCode, int nID, CWindow wndCtl)
{
    CloseDialog(0);
}

void CEditDlg::OnPathBrowse(UINT uNotifyCode, int nID, CWindow wndCtl)
{
    DoDataExchange(TRUE);

    CFileDialog dlg(TRUE, _T("exe"), NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, _T("Executable Files (*.exe)\0*.exe\0All Files (*.*)\0*.*\0"), m_hWnd);
    if (dlg.DoModal() != IDOK)
        return;

    m_pathText = dlg.m_szFileName;

    if (m_dirText.IsEmpty())
    {
        m_dirText = m_pathText.Left(m_pathText.ReverseFind(_T('\\')));
    }

    DoDataExchange(FALSE);
}

void CEditDlg::OnDirBrowse(UINT uNotifyCode, int nID, CWindow wndCtl)
{
    DoDataExchange(TRUE);

    CFolderDialog dlg(m_hWnd, _T("Choose the working directory:"));
    if (dlg.DoModal() != IDOK)
        return;

    m_dirText = dlg.m_szFolderPath;

    DoDataExchange(FALSE);
}
