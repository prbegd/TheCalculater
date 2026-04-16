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

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/third_party/catch2/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/third_party/CLI11/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/third_party/json5cpp/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/third_party/jsoncpp/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/third_party/libbacktrace/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/third_party/spdlog/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${BOOST_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC ${JSONCPP_LIBRARY})
target_link_libraries(${PROJECT_NAME} PUBLIC ${BACKTRACE_LIBRARY})

target_compile_definitions(${PROJECT_NAME} PUBLIC BOOST_STACKTRACE_USE_BACKTRACE)
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

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)