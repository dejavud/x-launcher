#include "stdafx.h"
#include "Task.h"
#include <TlHelp32.h>


CTask::CTask()
: m_hProcess(NULL)
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

    STARTUPINFO si = { 0 };
    si.cb = sizeof(si);
    si.wShowWindow = SW_HIDE;
    PROCESS_INFORMATION pi = { 0 };
    pi.hProcess = NULL;
    if (!::CreateProcess(NULL, (LPTSTR)(LPCTSTR)cmdline, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, (LPCTSTR)dir, &si, &pi))
    {
        return false;
    }

    ::CloseHandle(pi.hThread);
    m_hProcess = pi.hProcess;
    if (m_hProcess == NULL)
    {
        return false;
    }

    return true;
}

void CTask::Terminate()
{
    if (m_hProcess == NULL)
        return;

    DWORD pid = GetProcessId(m_hProcess);
    KillProcessTree(pid);
    ::CloseHandle(m_hProcess);
    m_hProcess = NULL;
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
        ::CloseHandle(m_hProcess);
        m_hProcess = NULL;
        return false;
    }
}

int StartedTaskNum(CTaskList& taskList)
{
    int num = 0;

    for (CTaskList::iterator it = taskList.begin(); it != taskList.end(); it++) {
        CTask& task = *it;
        if (task.CheckIfRunning())
            num++;
    }

    return num;
}
