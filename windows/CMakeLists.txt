cmake_minimum_required(VERSION 3.9)
project(jwm LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
    cc/ContextWGL.cc
    cc/ContextWGL.hh
    cc/LayerGL.cc
    cc/LayerGL.hh
    cc/PlatformWin32.hh
    cc/ScreenWin32.hh
    cc/WindowManagerWin32.cc
    cc/WindowManagerWin32.hh
    cc/WindowWin32.cc
    cc/WindowWin32.hh)

add_library(jwm SHARED
    ${JWM_SHARED_SOURCES_CXX}
    ${JWM_SHARED_SOURCES_CXX_IMPL}
    ${JWM_WINDOWS_SOURCES_CXX})

target_include_directories(jwm PRIVATE ${JWM_SHARED}/cc)
target_include_directories(jwm PRIVATE ${CMAKE_CURRENT_LIST_DIR}/cc)
target_include_directories(jwm PRIVATE ${JAVA_HOME}/include ${JAVA_HOME}/include/win32)

target_link_libraries(jwm PRIVATE "shcore")
target_link_libraries(jwm PRIVATE "gdi32")
target_link_libraries(jwm PRIVATE "opengl32")
target_link_libraries(jwm PRIVATE "user32")
target_link_libraries(jwm PRIVATE "Dxva2")
set_target_properties(jwm PROPERTIES OUTPUT_NAME "jwm_${JWM_ARCH}")