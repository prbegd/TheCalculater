# ! Configuration with '-D BOOST_INCLUDE_DIR=/path/to/boost/include'!

cmake_minimum_required(VERSION 3.16)

project(TheCalculater VERSION 0.0.1 LANGUAGES CXX)

if(POLICY CMP0167)
  cmake_policy(SET CMP0167 NEW)
endif()
if(POLICY CMP0144)
  cmake_policy(SET CMP0144 NEW)
endif()

if(NOT DEFINED BOOST_INCLUDE_DIR OR "${BOOST_INCLUDE_DIR}" STREQUAL "")
    message(FATAL_ERROR "Please configure with '-D BOOST_INCLUDE_DIR=/path/to/boost/include'.")
endif()

if(WIN32 AND NOT MINGW)
    message(FATAL_ERROR "Use mingw compiler in Windows!")
endif()

set(Boost_NO_SYSTEM_PATHS ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions -frtti -fexec-charset=UTF-8 -finput-charset=UTF-8 -fno-omit-frame-pointer -g")

set(CMAKE_COLOR_MAKEFILE ON)
set(CMAKE_COLOR_DIAGNOSTICS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
find_package(Qt6 REQUIRED COMPONENTS LinguistTools)
# find_package(Boost REQUIRED)

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

add_subdirectory(subprojects/CrashHandler)

file(GLOB_RECURSE PROJECT_SOURCES
"src/*.cpp"
"include/*.ui"
"include/*.h"
"include/*.hpp"
)
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

target_compile_options(${PROJECT_NAME} PRIVATE -fexceptions -frtti -fexec-charset=UTF-8 -finput-charset=UTF-8 -fno-omit-frame-pointer -g)
if(MINGW)
    target_compile_options(${PROJECT_NAME} PRIVATE -Wa,-mbig-obj)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -rdynamic)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Core)
target_link_libraries(${PROJECT_NAME} PRIVATE ${BACKTRACE_LIBRARY})
target_link_libraries(${PROJECT_NAME} PRIVATE ${JSONCPP_LIBRARY})

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

install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(${PROJECT_NAME})
endif()


enable_testing()
# add_dependencies(${PROJECT_NAME} resources)
add_dependencies(${PROJECT_NAME} resources CrashHandler)

target_precompile_headers(${PROJECT_NAME} PRIVATE "include/spdlog/pch.hpp")

target_compile_definitions(${PROJECT_NAME} PRIVATE SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE)
target_compile_definitions(${PROJECT_NAME} PRIVATE BOOST_STACKTRACE_USE_BACKTRACE)
# TODO: target_compile_definitions(${PROJECT_NAME} PRIVATE THECALCULATER_APP)
if(MINGW)
    # supports windows stack iterate （我也不知道有什么用）
    target_compile_definitions(${PROJECT_NAME} PRIVATE _WIN32_WINNT=0x0600)
endif()
if(APPLE) # clang
    target_compile_definitions(${PROJECT_NAME} PRIVATE _GNU_SOURCE)
endif()