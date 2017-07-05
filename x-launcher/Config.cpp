#include "stdafx.h"
#include "Config.h"
#include <fstream>

#undef max
#undef min
#include "json/json.hpp"

using namespace std;
using json = nlohmann::json;

#define CONFIG_FILE_NAME        "config.json"
#define CONFIG_TASKS            "tasks"
#define CONFIG_TASK_NAME        "name"
#define CONFIG_TASK_PATH        "path"
#define CONFIG_TASK_ARGS        "args"
#define CONFIG_TASK_DIR         "dir"
#define CONFIG_TASK_AUTOSTART   "autostart"
#define CONFIG_TASK_AUTORESTART "autorestart"

CConfig::CConfig()
: m_runAtStartup(false)
{
}

CConfig::~CConfig()
{
}

bool CConfig::Load()
{
    ifstream f(GetConfigPath());
    if (!f.is_open())
        return false;
    try
    {
        json j;
        f >> j;

        auto tasks = j.at(CONFIG_TASKS);
        if (!tasks.is_array())
            return false;

        m_taskList.clear();
        for (json::iterator it = tasks.begin(); it != tasks.end(); ++it) {
            if (!it->is_object())
                continue;
            string name = it->at(CONFIG_TASK_NAME).get<std::string>();
            string path = it->at(CONFIG_TASK_PATH).get<std::string>();
            string dir = it->at(CONFIG_TASK_DIR).get<std::string>();
            string args = it->at(CONFIG_TASK_ARGS).get<std::string>();
            bool autostart = it->at(CONFIG_TASK_AUTOSTART).get<bool>();
            bool autorestart = it->at(CONFIG_TASK_AUTORESTART).get<bool>();

            CTask task;
            task.name = U8toCS(name.c_str());
            task.path = U8toCS(path.c_str());
            task.dir = U8toCS(dir.c_str());
            task.args = U8toCS(args.c_str());
            task.autostart = autostart;
            task.autorestart = autorestart;
            m_taskList.push_back(task);
        }
     }
    catch (const std::exception& e)
    {
        const char* errstr = e.what();
        return false;
    }

    return true;
}

bool CConfig::Save()
{
    try
    {
        json j;
        json tasks;
        for (CTaskList::iterator it = m_taskList.begin(); it != m_taskList.end(); it++) {
            json t;
            t[CONFIG_TASK_NAME] = CStoU8((LPCTSTR)it->name);
            t[CONFIG_TASK_PATH] = CStoU8((LPCTSTR)it->path);
            t[CONFIG_TASK_DIR] = CStoU8((LPCTSTR)it->dir);
            t[CONFIG_TASK_ARGS] = CStoU8((LPCTSTR)it->args);
            t[CONFIG_TASK_AUTOSTART] = it->autostart;
            t[CONFIG_TASK_AUTORESTART] = it->autorestart;
            tasks.push_back(t);
        }
        j[CONFIG_TASKS] = tasks;

        ofstream f(GetConfigPath());
        if (!f.is_open())
            return false;

        f << j.dump(4);
    }
    catch (const std::exception& e)
    {
        const char* errstr = e.what();
        return false;
    }
    
    return true;
}

bool CConfig::GetRunAtStartup() const
{
    return m_runAtStartup;
}

void CConfig::SetRunAtStartup(bool runAtStartup)
{
    m_runAtStartup = runAtStartup;
}

CTaskList& CConfig::GetTaskList()
{
    return m_taskList;
}

CString CConfig::U8toCS(const char* str, int len/* = -1*/)
{
    ATLASSERT(str != nullptr);

    wstring wstr;
    int num_required, num_written;
    num_required = ::MultiByteToWideChar(CP_UTF8, 0, str, len, NULL, 0);
    if (num_required > 0) {
        wchar_t* wbuffer = new wchar_t[num_required];  // include terminating null character
        num_written = ::MultiByteToWideChar(CP_UTF8, 0, str, len, wbuffer, num_required);
        ATLASSERT(num_written == num_required);
        wstr.assign(wbuffer);
        delete[] wbuffer;
    }

    CString cstr;
#if defined UNICODE || defined _UNICODE
    cstr = wstr.c_str();
#else
    num_required = ::WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.length() + 1, NULL, 0, NULL, NULL);
    if (num_required > 0) {
        char* buffer = new char[num_required];
        num_written = ::WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.length() + 1, buffer, num_required, NULL, NULL);
        ATLASSERT(num_written == num_required);
        cstr = buffer;
        delete[] buffer;
    }
#endif

    return cstr;
}

std::string CConfig::CStoU8(LPCTSTR tstr, int len/* = -1*/)
{
    wstring wstr;
    int num_required, num_written;
#if defined UNICODE || defined _UNICODE
    wstr = tstr;
#else
    num_required = ::MultiByteToWideChar(CP_ACP, 0, tstr, len, NULL, 0);
    if (num_required > 0) {
        wchar_t* wbuffer = new wchar_t[num_required];
        num_written = ::MultiByteToWideChar(CP_ACP, 0, tstr, len, wbuffer, num_required);
        ATLASSERT(num_written == num_required);
        wstr.assign(wbuffer);
        delete[] wbuffer;
    }
#endif

    string str;
    num_required = ::WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), wstr.length() + 1, NULL, 0, NULL, NULL);
    if (num_required <= 0)
        return str;

    char* buffer = new char[num_required];  // include terminating null character
    num_written = ::WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), wstr.length() + 1, buffer, num_required, NULL, NULL);
    ATLASSERT(num_written == num_required);
    str.assign(buffer);
    delete[] buffer;

    return str;
}

void CConfig::ParseCmdline()
{
    int argc = 0;
    LPWSTR* argv = ::CommandLineToArgvW(::GetCommandLineW(), &argc);
    if (argv == NULL)
        return;

    for (int i = 1; i < argc; i++) {
        wstring s = argv[i];
        
    }
}

std::string CConfig::GetConfigPath()
{
    std::string path;

    char appPath[MAX_PATH];
    ::GetModuleFileNameA(NULL, appPath, MAX_PATH);
    CStringA tmp = appPath;
    int index = tmp.ReverseFind('\\');
    if (index == -1)
        index = tmp.ReverseFind('/');
    path = tmp.Left(index);

    path += '/';
    path += CONFIG_FILE_NAME;

    return path;
}
