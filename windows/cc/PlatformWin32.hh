#pragma once

#include <cwctype>
#include <windows.h>
#include <winuser.h>
#include <shellscalingapi.h>

#ifndef JWM_TRUE
 #define JWM_TRUE 1
#endif //JWM_TRUE

#ifndef JWM_FALSE
 #define JWM_FALSE 0
#endif //JWM_FALSE

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