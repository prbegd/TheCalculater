# Copyright © 2025 Cai Yaoxing
# SPDX-License-Identifier: GPL-3.0-only
# This file is part of TheCalculater.
# See the file LICENSE in the project root or go to 
# <https://www.gnu.org/licenses/gpl-3.0.html> for detailed license information.
project(ThirdPartyModulesMock)

add_library(${PROJECT_NAME} SHARED)
target_sources(${PROJECT_NAME}
PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${CMAKE_SOURCE_DIR}/include FILES
    ${CMAKE_SOURCE_DIR}/include/ThirdPartyModulesMock/boost.cppm
    ${CMAKE_SOURCE_DIR}/include/ThirdPartyModulesMock/CLI11.cppm
    ${CMAKE_SOURCE_DIR}/include/ThirdPartyModulesMock/cstd.cppm
    ${CMAKE_SOURCE_DIR}/include/ThirdPartyModulesMock/winapi.cppm
    ${CMAKE_SOURCE_DIR}/include/ThirdPartyModulesMock/posixapi.cppm
    ${CMAKE_SOURCE_DIR}/include/ThirdPartyModulesMock/spdlog.cppm
    ${CMAKE_SOURCE_DIR}/include/ThirdPartyModulesMock/jsoncpp.cppm
    ${CMAKE_SOURCE_DIR}/include/ThirdPartyModulesMock/json5cpp.cppm
)
target_sources(${PROJECT_NAME}
PRIVATE
    spdlog.cpp
    spdloglib.cpp
    winapi.cpp
    boost.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/third_party/json5cpp/include)

target_link_libraries(${PROJECT_NAME} PUBLIC libbacktrace::libbacktrace)
target_link_libraries(${PROJECT_NAME} PUBLIC Boost::headers)
target_link_libraries(${PROJECT_NAME} PUBLIC JsonCpp::JsonCpp)
target_link_libraries(${PROJECT_NAME} PUBLIC spdlog::spdlog_header_only)
target_link_libraries(${PROJECT_NAME} PUBLIC fmt::fmt)
target_link_libraries(${PROJECT_NAME} PUBLIC CLI11::CLI11)

target_compile_definitions(${PROJECT_NAME} PUBLIC BOOST_STACKTRACE_USE_BACKTRACE)
target_compile_definitions(${PROJECT_NAME} PUBLIC SPDLOG_COMPILED_LIB)
target_compile_definitions(${PROJECT_NAME} PUBLIC SPDLOG_SHARED_LIB)
target_compile_definitions(${PROJECT_NAME} PRIVATE spdlog_EXPORTS)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME} PUBLIC SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE)
else()
    target_compile_definitions(${PROJECT_NAME} PUBLIC SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_DEBUG)
endif()
# To prevent {fmt} from calling #pragma optimize
target_compile_definitions(${PROJECT_NAME} PUBLIC __OPTIMIZE__) 
if(APPLE) # clang
    target_compile_definitions(${PROJECT_NAME} PUBLIC _GNU_SOURCE)
endif()

target_compile_options(${PROJECT_NAME} PRIVATE -fexceptions -frtti -fexec-charset=UTF-8 -finput-charset=UTF-8 -fno-omit-frame-pointer -g)
if(WIN32)
    target_compile_options(${PROJECT_NAME} PRIVATE -Wa,-mbig-obj -fseh-exceptions)
endif()
target_compile_options(${PROJECT_NAME} PRIVATE "-Wno-ignored-attributes")