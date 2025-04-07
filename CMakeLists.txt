cmake_minimum_required(VERSION 3.16)

project(TheCalculater VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions -frtti -fexec-charset=UTF-8 -finput-charset=UTF-8 -fno-omit-frame-pointer -Wa,-mbig-obj")

if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug" AND NOT CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
endif()

set(CMAKE_COLOR_MAKEFILE ON)
set(CMAKE_COLOR_DIAGNOSTICS ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS LinguistTools)

qt_standard_project_setup(I18N_TRANSLATED_LANGUAGES zh_CN)

file(GLOB_RECURSE PROJECT_SOURCES 
"${PROJECT_SOURCE_DIR}/src/*.cpp"
"${PROJECT_SOURCE_DIR}/include/*.ui"
"${PROJECT_SOURCE_DIR}/include/*.h"
"${PROJECT_SOURCE_DIR}/include/*.hpp"
)
include_directories("${PROJECT_SOURCE_DIR}/include")

# qrc文件
set(RC_FILE "resources.rc")

# 生成 .rcc 文件
qt_add_binary_resources(resources resources.qrc DESTINATION ${CMAKE_SOURCE_DIR}/bin/resources.rcc)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(${PROJECT_NAME}
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ${RC_FILE}
    )
else()
    message(FATAL_ERROR "Should use qt6")
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Core)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
# if(${QT_VERSION} VERSION_LESS 6.1.0)
#   set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.${PROJECT_NAME})
# endif()
set_target_properties(${PROJECT_NAME} PROPERTIES
    ${BUNDLE_ID_OPTION}
    # MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    # MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    # MACOSX_BUNDLE TRUE
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
add_dependencies(${PROJECT_NAME} resources update_translations)


# 库
include_directories("${PROJECT_SOURCE_DIR}/third_party/spdlog/include")
include_directories("${PROJECT_SOURCE_DIR}/third_party/CLI11/include")
target_precompile_headers(${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/include/spdlog/pch.h")
target_link_libraries(${PROJECT_NAME} PRIVATE stdc++exp)

add_compile_definitions(SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE)
add_compile_definitions(PROGRAM_VERSION="${PROJECT_VERSION}")
add_compile_definitions(EXECUTABLE_NAME="${PROJECT_NAME}")

# target_link_libraries(${PROJECT_NAME} PRIVATE DbgHelp.lib)


add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/bin
    COMMAND objcopy --only-keep-debug ${PROJECT_NAME}.exe ${PROJECT_NAME}.debug
    # COMMAND cv2pdb ${PROJECT_NAME}.exe temp.exe ${PROJECT_NAME}.pdb
    # COMMAND ${CMAKE_COMMAND} -E remove temp.exe
    COMMAND objcopy --strip-debug ${PROJECT_NAME}.exe
    COMMAND objcopy --add-gnu-debuglink=${PROJECT_NAME}.debug ${PROJECT_NAME}.exe
    COMMENT "* Processing debug symbols..."
)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/bin 
    COMMAND windeployqt "${PROJECT_NAME}.exe" --verbose=0
    COMMENT "* Deploying DLLs..."
)