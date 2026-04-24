# Copyright © 2025 Cai Yaoxing
# SPDX-License-Identifier: GPL-3.0-only
# This file is part of TheCalculater.
# See the file LICENSE in the project root or go to 
# <https://www.gnu.org/licenses/gpl-3.0.html> for detailed license information.
project(CrashHandler VERSION 0.0.1)

qt_add_executable(${PROJECT_NAME}
    MANUAL_FINALIZATION
    crash_handler_dialog.cpp
    crash_handler_dialog.hpp
    crash_handler_dialog.ui
    main.cpp
)

target_compile_options(${PROJECT_NAME} PRIVATE -fexceptions -frtti -fexec-charset=UTF-8 -finput-charset=UTF-8 -fno-omit-frame-pointer -g)
if(WIN32)
    target_compile_options(${PROJECT_NAME} PRIVATE -Wa,-mbig-obj -fseh-exceptions)
endif()

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_BINARY_DIR}/include)

target_link_libraries(${PROJECT_NAME} PRIVATE TheCalculaterCommon)
target_link_libraries(${PROJECT_NAME} PRIVATE TheCalculaterQtBridge)
target_link_libraries(${PROJECT_NAME} PRIVATE ThirdPartyModulesMock)

set_target_properties(${PROJECT_NAME} PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(${PROJECT_NAME})
endif()