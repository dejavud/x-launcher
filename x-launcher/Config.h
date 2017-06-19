#pragma once

#include "Task.h"

class CConfig
{
public:
    CConfig();
    ~CConfig();

public:
    bool Load();
    bool Save();

    void ParseCmdline();

public:
    bool GetRunAtStartup() const;
    void SetRunAtStartup(bool runAtStartup);
    CTaskList& GetTaskList();
    bool GetAutoStart() const;

private:
    std::string GetConfigPath();
    CString U8toCS(const char* str, int len = -1);
    std::string CStoU8(LPCTSTR tstr, int len = -1);

private:
    bool m_runAtStartup;
    bool m_autoStart;
    CTaskList m_taskList;
};
