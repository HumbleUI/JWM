cmake_minimum_required(VERSION 3.9)
project(jwm LANGUAGES CXX OBJC)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT ARCH AND "${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "arm64")
    set(ARCH "arm64")
else()
    set(ARCH "x64")
endif()

file(GLOB SOURCES_OBJC ${CMAKE_SOURCE_DIR}/cc/*.mm)
file(GLOB SOURCES_CXX ${CMAKE_SOURCE_DIR}/../shared/cc/*.cc)
file(GLOB SOURCES_CXX_IMPL ${CMAKE_SOURCE_DIR}/../shared/cc/impl/*.cc)
add_library(jwm SHARED ${SOURCES_OBJC} ${SOURCES_CXX} ${SOURCES_CXX_IMPL})
target_include_directories(jwm PRIVATE ${CMAKE_SOURCE_DIR}/../shared/cc $ENV{JAVA_HOME}/include $ENV{JAVA_HOME}/include/darwin)
set_target_properties(jwm PROPERTIES OUTPUT_NAME "jwm_${ARCH}")

target_link_libraries(jwm
    "-framework AppKit"
    "-framework Cocoa"
    "-framework CoreData"
    "-framework Foundation"
    "-framework Metal"
    "-framework OpenGL"
    "-framework QuartzCore"
)