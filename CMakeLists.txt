cmake_minimum_required(VERSION 3.8)

## Project
project(ssp VERSION 0.1.0 LANGUAGES CXX)

## Subprojecs
add_subdirectory(examples)

## Target library
if(PLATFORM_IS_CYGWIN)
    add_library(${PROJECT_NAME} src/serial_win32.cpp)
elseif(UNIX)
    add_library(${PROJECT_NAME} src/serial_linux.cpp)
else()
    add_library(${PROJECT_NAME} src/serial_win32.cpp)
endif()

## Includes
target_include_directories(${PROJECT_NAME}
        PUBLIC
            $<INSTALL_INTERFACE:include>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/src
        )

## Install library
install(TARGETS
            ${PROJECT_NAME}
        EXPORT
            ${PROJECT_NAME}
        DESTINATION
            lib/ssp)

## Install headers
install(FILES
            include/ssp/serial.h
        DESTINATION
            include/ssp)

## Install CMake files
install(EXPORT
            ${PROJECT_NAME}
        DESTINATION
            lib/ssp)

install(FILES
            ${CMAKE_SOURCE_DIR}/cmake/${PROJECT_NAME}-config.cmake
        DESTINATION
            lib/ssp)
