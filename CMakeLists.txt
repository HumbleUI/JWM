cmake_minimum_required(VERSION 3.9)
project(jwm-project LANGUAGES CXX)

####################################################################
# Platform specific config
# Add subdirectories here to enable clion project index and support

if(APPLE)
    add_subdirectory(macos)
    message(STATUS "Configure macos platform module")
endif()

if(WIN32)
    add_subdirectory(windows)
    message(STATUS "Configure windows platform module")
endif()