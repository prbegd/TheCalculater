# Copyright © 2025 Cai Yaoxing
# SPDX-License-Identifier: GPL-3.0-only
# This file is part of TheCalculater.
# See the file LICENSE in the project root or go to 
# <https://www.gnu.org/licenses/gpl-3.0.html> for detailed license information.
project(TheCalculaterQtBridge)

add_library(${PROJECT_NAME} SHARED)

target_sources(${PROJECT_NAME}
PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${CMAKE_SOURCE_DIR}/include FILES
    ${CMAKE_SOURCE_DIR}/include/TheCalculaterQtBridge/resources.cppm
)
target_sources(${PROJECT_NAME}
PRIVATE
    resources.cpp
)

target_compile_options(${PROJECT_NAME} PRIVATE -fexceptions -frtti -fexec-charset=UTF-8 -finput-charset=UTF-8 -fno-omit-frame-pointer -g)
if(WIN32)
    target_compile_options(${PROJECT_NAME} PRIVATE -Wa,-mbig-obj)
endif()


target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/include)
# target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/third_party/spdlog/include)
# target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/third_party/CLI11/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/third_party/libbacktrace/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/third_party/json5cpp/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/third_party/jsoncpp/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_BINARY_DIR}/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${BOOST_INCLUDE_DIR})

target_include_directories(${PROJECT_NAME} PRIVATE TheCalculaterCommon)
# target_link_libraries(${PROJECT_NAME} PRIVATE ${JSONCPP_LIBRARY})
# target_link_libraries(${PROJECT_NAME} PRIVATE ${BACKTRACE_LIBRARY})
target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Core)

# target_compile_definitions(${PROJECT_NAME} PRIVATE BOOST_STACKTRACE_USE_BACKTRACE)
# To prevent {fmt} from calling #pragma optimize
target_compile_definitions(${PROJECT_NAME} PRIVATE __OPTIMIZE__) 
if(APPLE) # clang
    target_compile_definitions(${PROJECT_NAME} PRIVATE _GNU_SOURCE)
endif()
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME} PRIVATE SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE)
else()
    target_compile_definitions(${PROJECT_NAME} PRIVATE SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_DEBUG)
endif()