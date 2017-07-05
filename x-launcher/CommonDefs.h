#pragma once

#include <windows.h>

enum WndMessage {
    WM_TRAY_ICON = WM_USER + 1000,
    WM_EXIT_FROM_MENU,
    WM_HANDLE_EXCEPTION,
};

enum TimerID {
    TI_TASKS_PATROL = 0,
};

enum SubMenuType {
    SUB_MENU_TYPE_START = 0,
    SUB_MENU_TYPE_STOP,
    SUB_MENU_TYPE_EDIT,
    SUB_MENU_TYPE_DELETE,
    SUB_MENU_TYPE_SHOW,

    // end
    SUB_MENU_TOTAL_NUM,
};

enum ExceptionType {
    ET_UNKNOWN = 0,
    ET_SAVE_FAILURE,
};
