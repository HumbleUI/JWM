cmake_minimum_required(VERSION 3.9)
project(jwm LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (NOT MSVC)
    message(FATAL_ERROR "Support only MSVC C++ compiler for windows JWM library build.
                         Please, use MSVC toolchain.")
endif()

if(NOT JWM_ARCH)
    if ("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "arm64")
        set(JWM_ARCH "arm64")
    else()
        set(JWM_ARCH "x64")
    endif()
endif()

# Find jdk location to use jni includes
set(JAVA_HOME $ENV{JAVA_HOME})
if (JAVA_HOME)
    message(STATUS "Java home found by JAVA_HOME env variable: ${JAVA_HOME}")
else()
    message(FATAL_ERROR "Java home not found! Set JAVA_HOME env variable to provide correct path")
endif()

# Shared folder path
set(JWM_SHARED ${CMAKE_CURRENT_LIST_DIR}/../shared)

# Shared sources
file(GLOB JWM_SHARED_SOURCES_CXX ${JWM_SHARED}/cc/*.cc)
file(GLOB JWM_SHARED_SOURCES_CXX_IMPL ${JWM_SHARED}/cc/impl/*.cc)

# Windows related stuff
set(JWM_WINDOWS_SOURCES_CXX
    cc/AppWin32.cc
    cc/AppWin32.hh
    cc/ClipboardWin32.cc
    cc/ClipboardWin32.hh
    cc/ContextWGL.cc
    cc/ContextWGL.hh
    cc/LayerD3D12.cc
    cc/LayerD3D12.hh
    cc/LayerRaster.cc
    cc/LayerRaster.hh
    cc/LayerWGL.cc
    cc/LayerWGL.hh
    cc/ThemeHelperWin32.cc
    cc/ThemeHelperWin32.hh
    cc/PlatformWin32.hh
    cc/ScreenWin32.cc
    cc/ScreenWin32.hh
    cc/WindowManagerWin32.cc
    cc/WindowManagerWin32.hh
    cc/WindowWin32.cc
    cc/WindowWin32.hh)

# D3D12 Related stuff
set(JWM_DX12_SOURCES_CXX
    cc/D3D12/DX12CommandQueue.cc
    cc/D3D12/DX12CommandQueue.hh
    cc/D3D12/DX12Common.cc
    cc/D3D12/DX12Common.hh
    cc/D3D12/DX12Device.cc
    cc/D3D12/DX12Device.hh
    cc/D3D12/DX12Fence.cc
    cc/D3D12/DX12Fence.hh
    cc/D3D12/DX12SwapChain.cc
    cc/D3D12/DX12SwapChain.hh)

add_library(jwm SHARED
    ${JWM_SHARED_SOURCES_CXX}
    ${JWM_SHARED_SOURCES_CXX_IMPL}
    ${JWM_WINDOWS_SOURCES_CXX}
    ${JWM_DX12_SOURCES_CXX})

target_include_directories(jwm PRIVATE ${JWM_SHARED}/cc)
target_include_directories(jwm PRIVATE ${CMAKE_CURRENT_LIST_DIR}/cc)
target_include_directories(jwm PRIVATE ${JAVA_HOME}/include ${JAVA_HOME}/include/win32)

target_link_libraries(jwm PRIVATE "shcore")
target_link_libraries(jwm PRIVATE "gdi32")
target_link_libraries(jwm PRIVATE "opengl32")
target_link_libraries(jwm PRIVATE "user32")
target_link_libraries(jwm PRIVATE "Dxva2")
target_link_libraries(jwm PRIVATE "d3d12")
target_link_libraries(jwm PRIVATE "dxgi")
target_link_libraries(jwm PRIVATE "dxguid")
target_link_libraries(jwm PRIVATE "Imm32")

set_target_properties(jwm PROPERTIES OUTPUT_NAME "jwm_${JWM_ARCH}")

set(CMAKE_CXX_FLAGS_RELEASE "/MT")
set(CMAKE_CXX_FLAGS_DEBUG "/MTd")

target_compile_definitions(jwm
    PRIVATE WIN32
    PRIVATE _WIN32
    PRIVATE _WINDOWS
    PRIVATE UNICODE
    PRIVATE _UNICODE
    PRIVATE _CRT_SECURE_NO_WARNINGS
    PRIVATE _SCL_SECURE_NO_WARNINGS)

target_compile_options(jwm
    PRIVATE /source-charset:utf-8)

target_link_options(jwm
    PRIVATE /INCREMENTAL:NO
    PRIVATE /NODEFAULTLIB:MSVCRT
    PRIVATE /machine:X64)
