cmake_minimum_required(VERSION 3.0)
project(ssp)

set(CMAKE_CXX_STANDARD 14)

if (APPLE)
    add_library(${PROJECT_NAME} src/serial_macos.cpp)
elseif(UNIX)
    add_library(${PROJECT_NAME} src/serial_linux.cpp)
else()
    add_library(${PROJECT_NAME} src/serial_win32.cpp)
endif()

add_subdirectory(examples)
include_directories(include)