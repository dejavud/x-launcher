#pragma once

typedef void(*ReadOutputCallback)(const CString& str, UINT_PTR param);

class CTask
{
public:
    CString name;
    CString path;
    CString args;
    CString dir;
    bool autostart;
    bool autorestart;

public:
    CTask();
    ~CTask();

    bool Launch();
    void Terminate();
    bool IsLaunched() const;
    bool CheckIfRunning();

    void ReadOutput();
    const CString& GetTotalOutput() const;
    void SetReadOutputCallback(ReadOutputCallback cb, UINT_PTR param);

private:
    bool KillProcessTree(DWORD pid);
    bool CreateOuputPipe(HANDLE& hReadPipe, HANDLE& hWritePipe);
    void Cleanup();

private:
    HANDLE m_hProcess;
    HANDLE m_hReadPipe, m_hWritePipe;

    CString m_totalOutput;
    ReadOutputCallback m_readOutputCallback;
    UINT_PTR m_readOutputCallbackParam;
};
typedef std::vector<CTask> CTaskList;
