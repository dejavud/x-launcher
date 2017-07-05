#include "stdafx.h"
#include "Task.h"
#include <TlHelp32.h>

#define BUFSIZE 1024
#define OUTPUT_BUFFER_LIMIT (1024 * 32)

CTask::CTask()
: m_hProcess(NULL)
, m_hReadPipe(INVALID_HANDLE_VALUE)
, m_hWritePipe(INVALID_HANDLE_VALUE)
, m_readOutputCallback(NULL)
, m_readOutputCallbackParam(NULL)
{

}

CTask::~CTask()
{
    Terminate();
}

bool CTask::Launch()
{
    if (m_hProcess != NULL)
        return false;

    CString cmdline = path;
    if (!args.IsEmpty()) {
        cmdline += _T(" ");
        cmdline += args;
    }

    if (!CreateOuputPipe(m_hReadPipe, m_hWritePipe))
        return false;

    STARTUPINFO si = { 0 };
    si.cb = sizeof(STARTUPINFO);
    si.dwFlags = STARTF_USESTDHANDLES/* | STARTF_USESHOWWINDOW*/;
    //si.wShowWindow = SW_HIDE;
    si.hStdInput = ::GetStdHandle(STD_INPUT_HANDLE);
    si.hStdOutput = m_hWritePipe;
    si.hStdError = m_hWritePipe;
    PROCESS_INFORMATION pi = { 0 };
    pi.hProcess = NULL;
    if (!::CreateProcess(NULL, (LPTSTR)(LPCTSTR)cmdline, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, (LPCTSTR)dir, &si, &pi))
    {
        Cleanup();
        return false;
    }

    ::CloseHandle(pi.hThread);
    m_hProcess = pi.hProcess;
    if (m_hProcess == NULL)
    {
        Cleanup();
        return false;
    }

    return true;
}

void CTask::Terminate()
{
    if (m_hProcess == NULL)
        return;

    DWORD pid = ::GetProcessId(m_hProcess);
    KillProcessTree(pid);

    Cleanup();
}

bool CTask::CreateOuputPipe(HANDLE& hReadPipe, HANDLE& hWritePipe)
{
    if (hReadPipe != INVALID_HANDLE_VALUE) {
        ::CloseHandle(hReadPipe);
        hReadPipe = INVALID_HANDLE_VALUE;
    }
    if (hWritePipe != INVALID_HANDLE_VALUE) {
        ::CloseHandle(hWritePipe);
        hWritePipe = INVALID_HANDLE_VALUE;
    }

    SECURITY_ATTRIBUTES sa = { 0 };
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;

    if (!::CreatePipe(&hReadPipe, &hWritePipe, &sa, 0))
        return false;

    if (!::SetHandleInformation(hReadPipe, HANDLE_FLAG_INHERIT, 0))
        return false;

    return true;
}

bool CTask::KillProcessTree(DWORD pid)
{
    HANDLE hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (hProcess == NULL)
        return false;

    PROCESSENTRY32 pe;
    memset(&pe, 0, sizeof(PROCESSENTRY32));
    pe.dwSize = sizeof(PROCESSENTRY32);
    HANDLE hSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    // kill child processes
    if (::Process32First(hSnap, &pe)) {
        do {
            if (pe.th32ParentProcessID == pid)
                KillProcessTree(pe.th32ProcessID);
        } while (::Process32Next(hSnap, &pe));
    }

    ::TerminateProcess(hProcess, 1);
    ::CloseHandle(hProcess);
    return true;   
}

bool CTask::CheckIfRunning()
{
    if (m_hProcess == NULL)
        return false;

    DWORD exitCode = 0;
    if (!::GetExitCodeProcess(m_hProcess, &exitCode))
        return false;

    if (exitCode == STILL_ACTIVE) {
        return true;
    }
    else {
        Cleanup();
        return false;
    }
}

void CTask::Cleanup()
{
    ::CloseHandle(m_hProcess);
    m_hProcess = NULL;

    ::CloseHandle(m_hReadPipe);
    m_hReadPipe = INVALID_HANDLE_VALUE;
    ::CloseHandle(m_hWritePipe);
    m_hWritePipe = INVALID_HANDLE_VALUE;
}

void CTask::ReadOutput()
{
    CStringA strA;

    char buf[BUFSIZE] = { 0 };
    DWORD numRead = 0;
    DWORD numAvail = 0;
    do
    {
        if (m_hReadPipe == INVALID_HANDLE_VALUE)
            break;

        if (!PeekNamedPipe(m_hReadPipe, buf, BUFSIZE, &numRead, &numAvail, NULL))
            break;

        if (numAvail == 0)
            break;

        if (!::ReadFile(m_hReadPipe, buf, numAvail > BUFSIZE ? BUFSIZE : numAvail, &numRead, NULL))
            break;
        strA.Append(buf, numRead);

        numAvail -= numRead;
    } while (numAvail > 0);

    if (m_totalOutput.GetLength() > OUTPUT_BUFFER_LIMIT)
        m_totalOutput = m_totalOutput.Right(m_totalOutput.GetLength() - m_totalOutput.ReverseFind(_T('\n'))); // clear buffer

    CString str = CA2T(strA);
    m_totalOutput += str;

    if (m_readOutputCallback != NULL)
    {
        m_readOutputCallback(str, m_readOutputCallbackParam);
    }
}

const CString& CTask::GetTotalOutput() const
{
    return m_totalOutput;
}

void CTask::SetReadOutputCallback(ReadOutputCallback cb, UINT_PTR param)
{
    m_readOutputCallback = cb;
    m_readOutputCallbackParam = param;
}
