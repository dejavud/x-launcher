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
    void Terminate();
    bool IsRunning() const;

private:
    bool KillProcessTree(DWORD pid);

private:
    HANDLE m_hProcess;
};
typedef std::vector<CTask> CTaskList;
