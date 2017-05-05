#pragma once

#include <windows.h>

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
