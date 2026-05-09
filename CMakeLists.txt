# Copyright © 2025 Cai Yaoxing
# SPDX-License-Identifier: GPL-3.0-only
# This file is part of TheCalculater.
# See the file LICENSE in the project root or go to 
# <https://www.gnu.org/licenses/gpl-3.0.html> for detailed license information.

# CMake variables you CAN set for configuration:
# - `CMAKE_EXPERIMENTAL_CXX_IMPORT_STD`: The UUID to enable cmake experimental feature `import std;`. You can find the value for your cmake version at https://gitlab.kitware.com/cmake/cmake/-/blob/YOUR_CMAKE_VERSION/Help/dev/experimental.rst
# - `THIRDPARTY_CLANG_EXECUTABLE`: The clang compiler used in third party installation. Should be same as `CMAKE_C_COMPILER`.
# - `THIRDPARTY_CLANGXX_EXECUTABLE`: The clang++ compiler used in third party installation. Should be same as `CMAKE_CXX_COMPILER`.
# - `THIRDPARTY_GCC_EXECUTABLE`: (Windows only) The gcc compiler used in third party installation.
# - `THIRDPARTY_GXX_EXECUTABLE`: (Windows only) The g++ compiler used in third party installation.
# - `CONAN_HOST_PROFILE`: The Conan host profile for third party installation. Can be a Conan profile name or a file path. If cross compiling, this is the profile of the target machine.
# - `CONAN_BUILD_PROFILE`: The Conan build profile for third party installation. Can be a Conan profile name or a file path. If cross compiling, this is the profile of the building machine.

cmake_minimum_required(VERSION 3.31 FATAL_ERROR)

option(SKIP_DEPLOYMENT "Skip deployment of third party libraries and Qt plugins." OFF)
option(BUILD_TEST "Build tests executable." ON)

if(DEFINED CMAKE_EXPERIMENTAL_CXX_IMPORT_STD)
    # does nothing
elseif(CMAKE_VERSION LESS_EQUAL 3.31)
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

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

include("${CMAKE_SOURCE_DIR}/cmake/util.cmake")
set(CMAKE_PROJECT_TOP_LEVEL_INCLUDES "${CMAKE_SOURCE_DIR}/cmake/setup_third_party.cmake")

project(TheCalculater VERSION 0.1.1 LANGUAGES CXX)
# ! Release version please set PRE_RELEASE_VERSION to an empty string.
set(PRE_RELEASE_VERSION "")

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

set(CMAKE_LINKER_TYPE LLD)
if(NOT (${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang"))
    message(FATAL_ERROR "You have to use LLVM Clang to compile this project!")
endif()

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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

if(UNIX)
    find_package(Threads REQUIRED)
endif()

find_package(Qt6 REQUIRED)
find_package(Boost REQUIRED)
find_package(Catch2 REQUIRED)
find_package(CLI11 REQUIRED)
find_package(jsoncpp REQUIRED)
find_package(spdlog REQUIRED)
find_package(libbacktrace REQUIRED)

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

add_subdirectory(src/ThirdPartyModulesMock)
add_subdirectory(src/common)
add_subdirectory(src/QtBridge)
add_subdirectory(src/CrashHandler)
if(BUILD_TEST)
    add_subdirectory("test")
endif()
if(WIN32)
    add_subdirectory(src/HelperPipeReader)
endif()

set(RC_FILE "resources.rc")

qt_add_binary_resources(resources resources.qrc  DESTINATION ${CMAKE_SOURCE_DIR}/bin/resources.rcc)

configure_file(
  "${CMAKE_SOURCE_DIR}/config.h.in"
  "${CMAKE_BINARY_DIR}/include/config.h"
)

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

if(POSIX)
    # target_link_libraries(${PROJECT_NAME} PUBLIC libdl)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${CMAKE_DL_LIBS})
    target_link_libraries(${PROJECT_NAME} PUBLIC Threads::Threads)
endif()

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/third_party/json5cpp/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_BINARY_DIR}/include)

set_target_properties(${PROJECT_NAME} PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)
include(GNUInstallDirs)

function(deployDependencies)
    getConanFirstBinaryPackageId("qt/6.10.1" QT_CONAN_PACKAGE_ID)
    getConanPackageFolder(${QT_CONAN_PACKAGE_ID} QT_CONAN_BINARY_PACKAGE_FOLDER)
    if(WIN32)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E echo "Deploying Qt libraries and plugins..."
            COMMAND ${QT_CONAN_BINARY_PACKAGE_FOLDER}/bin/windeployqt "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.exe" --no-translations --no-network  --no-system-dxc-compiler --no-system-d3d-compiler --no-opengl-sw  --verbose=0
        ) 
    elseif(APPLE)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E echo "Deploying Qt libraries and plugins..."
            COMMAND ${QT_CONAN_BINARY_PACKAGE_FOLDER}/bin/macdeployqt "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.app" -verbose=0
        ) 
    else()
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E echo "Deploying Qt libraries and plugins..."
            COMMAND python ${CMAKE_SOURCE_DIR}/cmake/linuxdeployqt6.py "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}" -no-translations -verbose=0 -qtdir ${QT_CONAN_BINARY_PACKAGE_FOLDER}
        ) 
    endif()
endfunction()
if(NOT SKIP_DEPLOYMENT)
    deployDependencies()
endif()

qt_finalize_executable(${PROJECT_NAME})

add_dependencies(${PROJECT_NAME} ThirdPartyModulesMock TheCalculaterCommon TheCalculaterQtBridge resources CrashHandler)
if(WIN32)
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

if(APPLE) # macos
    target_compile_definitions(${PROJECT_NAME} PRIVATE _GNU_SOURCE)
endif()