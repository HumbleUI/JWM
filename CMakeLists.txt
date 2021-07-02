cmake_minimum_required(VERSION 3.9)
project(jwm-project LANGUAGES CXX)

####################################################################
# Platform specific config
# Add subdirectories here to enable clion project index and support

if(APPLE)
    add_subdirectory(macos)
endif()