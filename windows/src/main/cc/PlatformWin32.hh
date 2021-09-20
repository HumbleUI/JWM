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
#ifdef min
 #undef min
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

#ifndef JWM_WM_UI_CALLBACK_EVENT
 #define JWM_WM_UI_CALLBACK_EVENT 0x0ff01
#endif //JWM_WM_UI_CALLBACK_EVENT

#ifndef JWM_WM_FRAME_EVENT
 #define JWM_WM_FRAME_EVENT 0x0ff02
#endif //JWM_WM_FRAME_EVENT