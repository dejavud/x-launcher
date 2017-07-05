#include "stdafx.h"
#include "resource.h"
#include "OutputDlg.h"
#include "CommonDefs.h"

#define DEFAULT_DLG_WIDTH 800
#define DEFAULT_DLG_HEIGHT 600

COutputDlg::COutputDlg(CTask& task)
: m_task(task)
{

}

COutputDlg::~COutputDlg()
{
    
}

LRESULT COutputDlg::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    MoveWindow(0, 0, DEFAULT_DLG_WIDTH, DEFAULT_DLG_HEIGHT);
    CenterWindow();

    HICON hIcon = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON));
    SetIcon(hIcon, TRUE);
    HICON hIconSmall = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
    SetIcon(hIconSmall, FALSE);

    InitUI();

    return TRUE;
}

LRESULT COutputDlg::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    CloseDialog(0);
    return 0;
}

void COutputDlg::InitUI()
{
    ::SetWindowText(m_hWnd, (LPCTSTR)m_task.name);

    DoDataExchange(FALSE);

    RECT rcClient;
    GetClientRect(&rcClient);

    ::LoadLibrary(CRichEditCtrl::GetLibraryName());
    HWND hEditWnd = m_outputEdit.Create(m_hWnd, rcClient, NULL, 
        WS_CHILD | WS_VISIBLE/* | WS_BORDER*/ | WS_CLIPSIBLINGS | WS_CLIPCHILDREN |  WS_VSCROLL | WS_HSCROLL | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_MULTILINE | ES_READONLY, 0, IDC_EDIT_OUTPUT);
    ATLASSERT(hEditWnd != NULL);
    m_outputEdit.SetFont(GetFont());

    CHARFORMAT cf = { 0 };
    cf.cbSize = sizeof(CHARFORMAT);
    cf.dwMask = CFM_COLOR | CFM_SIZE | CFM_FACE;
    cf.yHeight = 240;  // font size: 12 (1/20)
    _tcscpy(cf.szFaceName, _T("Fixedsys"));
    cf.crTextColor = RGB(192, 192, 192);  // text color: gray
    m_outputEdit.SetCharFormat(cf, SCF_DEFAULT);
    m_outputEdit.SetBackgroundColor(RGB(0, 0, 0));  // background color: black

    CString str = m_task.GetTotalOutput();
    m_outputEdit.AppendText(str);
    m_outputEdit.ScrollCaret();

    m_task.SetReadOutputCallback(ReadOutputCallback, (UINT_PTR)this);

    DoDataExchange(FALSE);
}

void COutputDlg::CloseDialog(int val)
{
    m_task.SetReadOutputCallback(NULL, NULL);

    EndDialog(val);
}

void COutputDlg::ReadOutputCallback(const CString& str, UINT_PTR param)
{
    COutputDlg* pThis = (COutputDlg*)param;
    ATLASSERT(pThis != NULL);

    pThis->OnRefresh(str);
}

void COutputDlg::OnRefresh(const CString& str)
{
    if (str.IsEmpty())
        return;

    CString s = str;
    m_outputEdit.AppendText(s);
    m_outputEdit.ScrollCaret();

    DoDataExchange(FALSE);
}
