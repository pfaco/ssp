cmake_minimum_required(VERSION 3.8)

## Project
project(ssp_examples)

## Serial source
if (APPLE)
    set(SSP_EXAMPLES_SERIAL_SRC ../src/serial_macos.cpp)
elseif(UNIX)
    set(SSP_EXAMPLES_SERIAL_SRC ../src/serial_linux.cpp)
else()
    set(SSP_EXAMPLES_SERIAL_SRC ../src/serial_win32.cpp)
endif()

## Targets
add_executable(ssp_demo1 demo1.cpp ${SSP_EXAMPLES_SERIAL_SRC})

## Compiler Features
target_compile_features(ssp_demo1 PRIVATE cxx_std_14)

## Includes
target_include_directories(ssp_demo1 PRIVATE ../include)