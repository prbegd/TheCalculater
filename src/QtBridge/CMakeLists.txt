# Copyright © 2025 Cai Yaoxing
# SPDX-License-Identifier: GPL-3.0-only
# This file is part of TheCalculater.
# See the file LICENSE in the project root or go to 
# <https://www.gnu.org/licenses/gpl-3.0.html> for detailed license information.
project(TheCalculaterQtBridge)

add_library(${PROJECT_NAME} SHARED)

target_sources(${PROJECT_NAME}
PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${CMAKE_SOURCE_DIR}/include FILES
    ${CMAKE_SOURCE_DIR}/include/TheCalculaterQtBridge/qt.cppm
    ${CMAKE_SOURCE_DIR}/include/TheCalculaterQtBridge/resources.cppm
)
target_sources(${PROJECT_NAME}
PRIVATE
    resources.cpp
)

target_compile_options(${PROJECT_NAME} PRIVATE -fexceptions -frtti -fexec-charset=UTF-8 -finput-charset=UTF-8 -fno-omit-frame-pointer -g)
if(WIN32)
    target_compile_options(${PROJECT_NAME} PRIVATE -Wa,-mbig-obj -fseh-exceptions)
endif()


target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_BINARY_DIR}/include)

target_link_libraries(${PROJECT_NAME} PRIVATE TheCalculaterCommon)
target_link_libraries(${PROJECT_NAME} PRIVATE ThirdPartyModulesMock)
target_link_libraries(${PROJECT_NAME} PUBLIC qt::qt)