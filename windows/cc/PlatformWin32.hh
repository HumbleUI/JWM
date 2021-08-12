#pragma once

#include <cwctype>
#include <windows.h>
#include <winuser.h>
#include <imm.h>
#include <windowsx.h>
#include <shellscalingapi.h>
#include <highlevelmonitorconfigurationapi.h>

#ifdef max
 #undef max
#endif

#ifndef JWM_WIN32_WINDOW_CLASS_NAME
 #define JWM_WIN32_WINDOW_CLASS_NAME L"JWM_WINDOW"
#endif //JWM_WIN32_WINDOW_CLASS_NAME

#ifndef JWM_WIN32_WINDOW_DEFAULT_NAME
 #define JWM_WIN32_WINDOW_DEFAULT_NAME L""
#endif //JWM_WIN32_WINDOW_DEFAULT_NAME

#ifndef JWM_DEFAULT_SCREEN_DPI
 #define JWM_DEFAULT_SCREEN_DPI 96
#endif //JWM_DEFAULT_SCREEN_DPI

#ifndef JWM_DEFAULT_DEVICE_SCALE
 #define JWM_DEFAULT_DEVICE_SCALE SCALE_100_PERCENT
#endif //JWM_DEFAULT_DEVICE_SCALE

#ifndef JWM_WM_TIMER_UPDATE_EVENT
 #define JWM_WM_TIMER_UPDATE_EVENT 0x1
#endif //JWM_WM_TIMER_UPDATE_EVENT