#pragma once

class CTask
{
public:
    CString name;
    CString path;
    CString args;
    CString dir;

public:
    CTask();
    ~CTask();

    bool Launch();
    void Term();

private:
    bool KillProcessTree(DWORD pid);

private:
    HANDLE m_hProcess;
};
typedef std::vector<CTask> CTaskList;
