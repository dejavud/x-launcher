#pragma once

#include "Config.h"

class CTrayMenu
{
public:
    CTrayMenu(CConfig& config);
    ~CTrayMenu();

public:
    bool Create();
    void Show(HWND hWnd);

private:
    bool InitSubMenu(CMenuHandle& subMenu, UINT index);
    void PrepareMenu(CMenuHandle& menu);
    void PrepareSubMenu(CMenuHandle& subMenu, UINT index, CTask& task);

private:
    CConfig& m_config;
    CMenu m_menu;
};
