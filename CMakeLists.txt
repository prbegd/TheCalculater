# Configuration needs Environment Variable “BOOST_ROOT” (base directory of lib/ and include/)
# Use `$env:BOOST_ROOT=C:\path\to\boost` in Powershell
# or `export BOOST_ROOT=/path/to/boost` in Bash

cmake_minimum_required(VERSION 3.16)

project(TheCalculater VERSION 0.0.1 LANGUAGES CXX)

if(POLICY CMP0167)
  cmake_policy(SET CMP0167 NEW)
endif()
if(POLICY CMP0144)
  cmake_policy(SET CMP0144 NEW)
endif()

if(NOT DEFINED ENV{BOOST_ROOT} OR "$ENV{BOOST_ROOT}" STREQUAL "")
    message(FATAL_ERROR "Environment Variable BOOST_ROOT is not set!")
endif()

if(WIN32 AND NOT MINGW)
    message(FATAL_ERROR "Use mingw compiler in Windows!")
endif()

set(BOOST_ROOT $ENV{BOOST_ROOT})
set(Boost_NO_SYSTEM_PATHS ON)
# set(BOOST_INCLUDEDIR "${BOOST_ROOT}/include")
# set(BOOST_LIBRARYDIR "${BOOST_ROOT}/lib")

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions -frtti -fexec-charset=UTF-8 -finput-charset=UTF-8 -fno-omit-frame-pointer -g")

# set(Boost_DEBUG ON)
# set(Boost_LIB_PREFIX "lib")
# set(Boost_ARCHITECTURE "-x64") 

set(CMAKE_COLOR_MAKEFILE ON)
set(CMAKE_COLOR_DIAGNOSTICS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(UNIX)
    find_package(Threads REQUIRED)
endif()

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS LinguistTools)

set(BACKTRACE_INCLUDE_DIR "third_party/libbacktrace/include")
if(MINGW)
    find_library(BACKTRACE_LIBRARY NAMES backtrace libbacktrace)
    if(BACKTRACE_INCLUDE_DIR AND BACKTRACE_LIBRARY)
        add_library(Backtrace::Backtrace INTERFACE IMPORTED)
        target_include_directories(Backtrace::Backtrace INTERFACE ${BACKTRACE_INCLUDE_DIR})
        target_link_libraries(Backtrace::Backtrace INTERFACE ${BACKTRACE_LIBRARY})
    else()
        message(FATAL_ERROR "libbacktrace not found for MinGW!")
    endif()
else()
    find_package(Backtrace REQUIRED)
endif()

qt_standard_project_setup(I18N_TRANSLATED_LANGUAGES zh_CN)

file(GLOB_RECURSE PROJECT_SOURCES 
# "${PROJECT_SOURCE_DIR}/src/*.cpp"
# "${PROJECT_SOURCE_DIR}/include/*.ui"
# "${PROJECT_SOURCE_DIR}/include/*.h"
# "${PROJECT_SOURCE_DIR}/include/*.hpp"
"src/*.cpp"
"include/*.ui"
"include/*.h"
"include/*.hpp"
)


# qrc文件
set(RC_FILE "resources.rc")

# 生成 .rcc 文件
qt_add_binary_resources(resources resources.qrc DESTINATION ${CMAKE_SOURCE_DIR}/bin/resources.rcc)

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
target_link_libraries(${PROJECT_NAME} PRIVATE Backtrace::Backtrace)
# if(MINGW)
#     # target_link_libraries(${PROJECT_NAME} PUBLIC "${PROJECT_SOURCE_DIR}/third_party/libbacktrace_mingw/libbacktrace/libbacktrace.a")
# endif()
if(POSIX)
    # target_link_libraries(${PROJECT_NAME} PUBLIC libdl)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${CMAKE_DL_LIBS})
    target_link_libraries(${PROJECT_NAME} PUBLIC Threads::Threads)
endif()
# if (Backtrace_FOUND)
#     target_link_libraries(${PROJECT_NAME} PRIVATE ${Backtrace_LIBRARIES})
# elseif (BACKTRACE_LIBRARY)
#     target_link_libraries(${PROJECT_NAME} PRIVATE ${BACKTRACE_LIBRARY})
# else()
#     message(FATAL_ERROR "libbacktrace not found")
# endif()

# target_include_directories(${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/include")
# target_include_directories(${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/third_party/spdlog/include")
# target_include_directories(${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/third_party/CLI11/include")
# target_include_directories(${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/third_party/libbacktrace/include")
target_include_directories(${PROJECT_NAME} PRIVATE "include")
target_include_directories(${PROJECT_NAME} PRIVATE "third_party/spdlog/include")
target_include_directories(${PROJECT_NAME} PRIVATE "third_party/CLI11/include")

target_include_directories(${PROJECT_NAME} PRIVATE "third_party/libbacktrace/include")

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

qt_add_translations(${PROJECT_NAME} 
    TS_FILE_DIR "translations"
)

enable_testing()
add_dependencies(${PROJECT_NAME} resources)
# add_dependencies(${PROJECT_NAME} resources update_translations)

# 库
# target_precompile_headers(${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/include/spdlog/pch.hpp")
target_precompile_headers(${PROJECT_NAME} PRIVATE "include/spdlog/pch.hpp")

target_compile_definitions(${PROJECT_NAME} PRIVATE SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE)
target_compile_definitions(${PROJECT_NAME} PRIVATE THECALCULATER_VERSION="${PROJECT_VERSION}")
# add_compile_definitions(BOOST_STACKTRACE_LINK)
target_compile_definitions(${PROJECT_NAME} PRIVATE BOOST_STACKTRACE_USE_BACKTRACE)
# TODO: target_compile_definitions(${PROJECT_NAME} PRIVATE THECALCULATER_APP)
if(MINGW)
    target_compile_definitions(${PROJECT_NAME} PRIVATE _WIN32_WINNT=0x0600)  # 支持Windows栈遍历
endif()