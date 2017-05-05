#pragma once

#include <windows.h>

#define TRAY_ID 1000
#define TRAY_TOOLTIP _T("x-launcher by ด๓นท")

#define REG_RUNATSTARTUP_KEY_NAME _T("x-launcher")

enum WndMessage {
    WM_TRAY_ICON = WM_USER + 1000,
    WM_EXIT_FROM_MENU,
};

enum SubMenuType {
    SUB_MENU_TYPE_START = 0,
    SUB_MENU_TYPE_STOP,
    SUB_MENU_TYPE_EDIT,
    SUB_MENU_TYPE_DELETE,

    // end
    SUB_MENU_TOTAL_NUM,
};
