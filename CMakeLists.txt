# ! Configuration with '-D BOOST_INCLUDE_DIR=/path/to/boost/include'!

# Copyright © 2025 Cai Yaoxing
# SPDX-License-Identifier: GPL-3.0-only
# This file is part of TheCalculater.
# See the file LICENSE in the project root or go to 
# <https://www.gnu.org/licenses/gpl-3.0.html> for detailed license information.

cmake_minimum_required(VERSION 3.31 FATAL_ERROR)

if(CMAKE_VERSION LESS_EQUAL 3.31)
    set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD 0e5b6991-d74f-4b3d-a41c-cf096e0b2508)
elseif(CMAKE_VERSION LESS_EQUAL 4.0)
    set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD a9e1cf81-9932-4810-974b-6eccaf14e457)
elseif(CMAKE_VERSION LESS_EQUAL 4.2)
    set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD d0edc3af-4c50-42ea-a356-e2862fe7a444)
elseif(CMAKE_VERSION LESS_EQUAL 4.3)
    set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD 451f2fe2-a8a2-47c3-bc32-94786d8fc91b)
else()
    message(FATAL_ERROR "Don't know the value of CMAKE_EXPERIMENTAL_CXX_IMPORT_STD for CMake version ${CMAKE_VERSION}. Please set it manually.")
endif()
set(CMAKE_CXX_MODULE_STD ON)

project(TheCalculater VERSION 0.1.1 LANGUAGES CXX)
# ! Release version please set PRE_RELEASE_VERSION to an empty string.
set(PRE_RELEASE_VERSION "")

set(CMAKE_LINKER_TYPE LLD)

if(NOT ${PRE_RELEASE_VERSION} STREQUAL "")
    set(PROJECT_VERSION "${PROJECT_VERSION}-${PRE_RELEASE_VERSION}")
endif()
if(POLICY CMP0167)
  cmake_policy(SET CMP0167 NEW)
endif()
if(POLICY CMP0144)
  cmake_policy(SET CMP0144 NEW)
endif()
if(POLICY CMP0155)
  cmake_policy(SET CMP0155 NEW)
endif()

if(NOT DEFINED BOOST_INCLUDE_DIR OR "${BOOST_INCLUDE_DIR}" STREQUAL "")
    message(FATAL_ERROR "Please configure with '-D BOOST_INCLUDE_DIR=/path/to/boost/include'.")
endif()

set(Boost_NO_SYSTEM_PATHS ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")

set(CMAKE_COLOR_DIAGNOSTICS ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Qunused-arguments")

if(UNIX)
    set(CMAKE_SKIP_BUILD_RPATH FALSE)
    if (APPLE)
        set(CMAKE_BUILD_RPATH "@loader_path")
    else()
        set(CMAKE_BUILD_RPATH "\$ORIGIN")
    endif()
endif()

find_package(Git QUIET)
if(GIT_FOUND)
  execute_process(
    COMMAND ${GIT_EXECUTABLE} rev-list --count HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT_COUNT
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  
  execute_process(
    COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  
  set(BUILD_NUMBER "${GIT_COMMIT_COUNT}.${GIT_COMMIT_HASH}")
else()
  set(BUILD_NUMBER "unknown")
endif()

if(UNIX)
    find_package(Threads REQUIRED)
endif()

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

if(DEFINED BACKTRACE_LIBRARY_PATH)
    find_library(BACKTRACE_LIBRARY NAMES backtrace libbacktrace HINTS ${BACKTRACE_LIBRARY_PATH})
else()
    find_library(BACKTRACE_LIBRARY NAMES backtrace libbacktrace)
endif()
if(NOT BACKTRACE_LIBRARY)
    message(FATAL_ERROR "libbacktrace livrary not found!")
endif()

if(DEFINED JSONCPP_LIBRARY_PATH)
    find_library(JSONCPP_LIBRARY NAMES jsoncpp HINTS ${JSONCPP_LIBRARY_PATH})
else()
    find_library(JSONCPP_LIBRARY NAMES jsoncpp)
endif()
if(NOT JSONCPP_LIBRARY)
    message(FATAL_ERROR "jsoncpp library not found!")
endif()

add_subdirectory(src/ThirdPartyModulesMock)
add_subdirectory(src/common)
add_subdirectory(src/QtBridge)
add_subdirectory(src/CrashHandler)
add_subdirectory("test")
if(WIN32)
    add_subdirectory(src/HelperPipeReader)
endif()

set(RC_FILE "resources.rc")

qt_add_binary_resources(resources resources.qrc  DESTINATION ${CMAKE_SOURCE_DIR}/bin/resources.rcc)

configure_file(
  "${CMAKE_SOURCE_DIR}/cmake/config.h.in"
  "${CMAKE_BINARY_DIR}/include/config.h"
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    if (WIN32)
        qt_add_executable(${PROJECT_NAME}
            MANUAL_FINALIZATION
            ${PROJECT_SOURCES}
            ${RC_FILE}
        )
    else()
        qt_add_executable(${PROJECT_NAME}
            MANUAL_FINALIZATION
            ${PROJECT_SOURCES}
        )
    endif()
else()
    message(FATAL_ERROR "Should use qt6")
endif()

target_sources(${PROJECT_NAME}
PUBLIC FILE_SET CXX_MODULES BASE_DIRS src/TheCalculater FILES
    src/TheCalculater/debugging.cppm
)
target_sources(${PROJECT_NAME} PRIVATE
    src/TheCalculater/main.cpp
    src/TheCalculater/ui/mainwindow.cpp
    src/TheCalculater/ui/mainwindow.h
    src/TheCalculater/ui/mainwindow.ui
)

target_compile_options(${PROJECT_NAME} PRIVATE -fexceptions -frtti -fexec-charset=UTF-8 -finput-charset=UTF-8 -fno-omit-frame-pointer -g)
if(WIN32)
    target_compile_options(${PROJECT_NAME} PRIVATE -Wa,-mbig-obj -fseh-exceptions)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE ThirdPartyModulesMock)
target_link_libraries(${PROJECT_NAME} PRIVATE TheCalculaterCommon)
target_link_libraries(${PROJECT_NAME} PRIVATE TheCalculaterQtBridge)
target_link_libraries(${PROJECT_NAME} PRIVATE ${BACKTRACE_LIBRARY})
target_link_libraries(${PROJECT_NAME} PRIVATE ${JSONCPP_LIBRARY})
target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Core)

if(POSIX)
    # target_link_libraries(${PROJECT_NAME} PUBLIC libdl)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${CMAKE_DL_LIBS})
    target_link_libraries(${PROJECT_NAME} PUBLIC Threads::Threads)
endif()

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/third_party/spdlog/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/third_party/CLI11/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/third_party/libbacktrace/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/third_party/json5cpp/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/third_party/jsoncpp/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_BINARY_DIR}/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${BOOST_INCLUDE_DIR})


# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.${PROJECT_NAME})
endif()
set_target_properties(${PROJECT_NAME} PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)
include(GNUInstallDirs)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(${PROJECT_NAME})
endif()

enable_testing()
add_dependencies(${PROJECT_NAME} ThirdPartyModulesMock TheCalculaterCommon TheCalculaterQtBridge resources CrashHandler)
if (WIN32)
    add_dependencies(${PROJECT_NAME} HelperPipeReader)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME} PRIVATE SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE)
else()
    target_compile_definitions(${PROJECT_NAME} PRIVATE SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_DEBUG)
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE BOOST_STACKTRACE_USE_BACKTRACE)
# Windows 10 1809 "Redstone 5" and later
# See https://doc.qt.io/qt-6/windows.html
target_compile_definitions(${PROJECT_NAME} PRIVATE _WIN32_WINNT=0x0A00) 
target_compile_definitions(${PROJECT_NAME} PRIVATE NTDDI_VERSION=0x0A000006) 
# To prevent {fmt} from calling #pragma optimize
target_compile_definitions(${PROJECT_NAME} PRIVATE __OPTIMIZE__) 

if(APPLE) # clang
    target_compile_definitions(${PROJECT_NAME} PRIVATE _GNU_SOURCE)
endif()