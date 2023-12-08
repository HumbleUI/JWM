cmake_minimum_required(VERSION 3.9)

# prefer the newer GL library (GLVND)
cmake_policy(SET CMP0072 NEW)

project(jwm LANGUAGES CXX)
project(xdgShell LANGUAGES C)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT JWM_ARCH)
    if ("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "arm64")
        set(JWM_ARCH "arm64")
    else()
        set(JWM_ARCH "x64")
    endif()
endif()

file(GLOB SOURCES_CXX ${CMAKE_CURRENT_LIST_DIR}/../shared/cc/*.cc 
    ${CMAKE_CURRENT_LIST_DIR}/../linux/cc/*.cc
    ${CMAKE_CURRENT_LIST_DIR}/cc/*.cc )
file(GLOB SOURCES_CXX_IMPL ${CMAKE_CURRENT_LIST_DIR}/../shared/cc/impl/*.cc)
add_library(jwm SHARED ${SOURCES_OBJC} ${SOURCES_CXX} ${SOURCES_CXX_IMPL})
find_library(WAYLAND_CLIENT_LIB wayland-client)
find_library(DECOR_LIB decor-0)
find_library(WAYLAND_CURSOR wayland-cursor)
find_library(XKBCOMMON xkbcommon)
find_library(EGL EGL)
find_library(WAYLAND_EGL wayland-egl)
# find_library(OPENGL_ES2 GLESv2)
find_package(OpenGL REQUIRED)
set(JAVA_HOME $ENV{JAVA_HOME})
if (NOT JAVA_HOME)
    file(GLOB JAVA_HOMES "/usr/lib/jvm/java-*")
    if (JAVA_HOMES)
        list(GET JAVA_HOMES 0 JAVA_HOME)
        message(STATUS "Java home found automatically at ${JAVA_HOME}. Set JAVA_HOME environment variable to override.")
    else()
        message(FATAL_ERROR "Java home not found! Please set JAVA_HOME environment variable.")
    endif()
endif()

target_include_directories(jwm PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../shared/cc ${CMAKE_CURRENT_LIST_DIR}/../linux/cc ${JAVA_HOME}/include ${JAVA_HOME}/include/linux)
set_target_properties(jwm PROPERTIES OUTPUT_NAME "jwm_${JWM_ARCH}_wayland")

target_link_libraries(jwm PRIVATE ${WAYLAND_CLIENT_LIB} ${DECOR_LIB}
    ${WAYLAND_CURSOR} ${XKBCOMMON})
target_link_libraries(jwm PRIVATE ${EGL} ${WAYLAND_EGL})
target_link_libraries(jwm PRIVATE OpenGL::GL)

