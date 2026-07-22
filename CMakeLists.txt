 # Copyright © 2026 Cai Yaoxing
 # 
 # This file is part of TheCalculater.
 # TheCalculater is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 # TheCalculater is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 # You should have received a copy of the GNU General Public License along with TheCalculater. If not, see <https://www.gnu.org/licenses/>.

# CMake variables you CAN set for configuration:
# - `CMAKE_EXPERIMENTAL_CXX_IMPORT_STD`: The UUID to enable cmake experimental feature `import std;`. You can find the value for your cmake version at https://gitlab.kitware.com/cmake/cmake/-/blob/YOUR_CMAKE_VERSION/Help/dev/experimental.rst
# - `THIRDPARTY_CLANG_EXECUTABLE`: The clang compiler used in third party installation. Should be same as `CMAKE_C_COMPILER`.
# - `THIRDPARTY_CLANGXX_EXECUTABLE`: The clang++ compiler used in third party installation. Should be same as `CMAKE_CXX_COMPILER`.
# - `THIRDPARTY_GCC_EXECUTABLE`: (Windows only) The gcc compiler used in third party installation.
# - `THIRDPARTY_GXX_EXECUTABLE`: (Windows only) The g++ compiler used in third party installation.
# - `CONAN_HOST_PROFILE`: The Conan host profile for third party installation. Can be a Conan profile name or a file path. If cross compiling, this is the profile of the target machine.
# - `CONAN_BUILD_PROFILE`: The Conan build profile for third party installation. Can be a Conan profile name or a file path. If cross compiling, this is the profile of the building machine.
# - `DUMP_PROJECT_VERSION`: If defined, the project version will be written to the file specified by this variable.

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
elseif(CMAKE_VERSION LESS_EQUAL 4.4)
    set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD 451f2fe2-a8a2-47c3-bc32-94786d8fc91b)
else()
    message(FATAL_ERROR "Don't know the value of CMAKE_EXPERIMENTAL_CXX_IMPORT_STD for CMake version ${CMAKE_VERSION}. Please set it manually.")
endif()
set(CMAKE_CXX_MODULE_STD ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

include("${CMAKE_SOURCE_DIR}/cmake/util.cmake")
set(CMAKE_PROJECT_TOP_LEVEL_INCLUDES "${CMAKE_SOURCE_DIR}/cmake/setup_third_party.cmake")

project(TheCalculater VERSION 0.2.0 LANGUAGES CXX)
# ! Release version please set PRE_RELEASE_VERSION to an empty string.
set(PRE_RELEASE_VERSION "unreleased")
 
if(NOT ${PRE_RELEASE_VERSION} STREQUAL "")
    set(PROJECT_VERSION "${PROJECT_VERSION}-${PRE_RELEASE_VERSION}")
endif()
if(DEFINED DUMP_PROJECT_VERSION)
    file(WRITE "${DUMP_PROJECT_VERSION}" "${PROJECT_VERSION}")
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

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Qunused-arguments -Wall -Wextra -Wpedantic -Wno-reserved-module-identifier")

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

find_package(Boost REQUIRED)
find_package(Catch2 REQUIRED)
find_package(CLI11 REQUIRED)
find_package(gmp REQUIRED)
find_package(jsoncpp REQUIRED)
find_package(libbacktrace REQUIRED)
find_package(Qt6 REQUIRED)
find_package(spdlog REQUIRED)

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

add_subdirectory(src/TheCalculater)
add_subdirectory(src/ThirdParty)
add_subdirectory(src/TheCalculaterCore)
add_subdirectory(src/CrashHandler)
if(BUILD_TEST)
    add_subdirectory("test")
endif()
if(WIN32)
    add_subdirectory(src/HelperPipeReader)
endif()

qt_add_binary_resources(resources resources.qrc DESTINATION ${CMAKE_SOURCE_DIR}/bin/resources.rcc)

configure_file(
  "${CMAKE_SOURCE_DIR}/config.h.in"
  "${CMAKE_BINARY_DIR}/include/config.h"
)

include(GNUInstallDirs)

add_dependencies(TheCalculater resources)