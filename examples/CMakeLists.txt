cmake_minimum_required(VERSION 3.10)
project(ssp_demo1)

set(CMAKE_CXX_STANDARD 17)

if (APPLE)
    
elseif(UNIX)
    add_executable(ssp_demo1 demo1.cpp ../src/serial_linux.cpp)
else()
    add_executable(ssp_demo1 demo1.cpp ../src/serial_win32.cpp)
endif()

include_directories(../include)