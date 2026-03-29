# Copyright © 2025 Cai Yaoxing
# SPDX-License-Identifier: GPL-3.0-only
# This file is part of TheCalculater.
# See the file LICENSE in the project root or go to 
# <https://www.gnu.org/licenses/gpl-3.0.html> for detailed license information.
project(TheCalculaterCommon)

add_library(${PROJECT_NAME} SHARED)
# Module Interface Unit files
target_sources(${PROJECT_NAME}
PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${CMAKE_SOURCE_DIR}/include FILES
    ${CMAKE_SOURCE_DIR}/include/TheCalculater/throwEx.cppm
    ${CMAKE_SOURCE_DIR}/include/TheCalculater/libTheCalculaterCommon.cppm
    ${CMAKE_SOURCE_DIR}/include/TheCalculater/util/-.cppm
    ${CMAKE_SOURCE_DIR}/include/TheCalculater/util/exceptions.cppm
    ${CMAKE_SOURCE_DIR}/include/TheCalculater/util/hasher.cppm
    ${CMAKE_SOURCE_DIR}/include/TheCalculater/util/json.cppm
    ${CMAKE_SOURCE_DIR}/include/TheCalculater/util/memory.cppm
    ${CMAKE_SOURCE_DIR}/include/TheCalculater/util/thread.cppm
    ${CMAKE_SOURCE_DIR}/include/TheCalculater/util/io.cppm
    ${CMAKE_SOURCE_DIR}/include/TheCalculater/util/constexpr_string.cppm
    ${CMAKE_SOURCE_DIR}/include/TheCalculater/util/shortcuts.cppm
    ${CMAKE_SOURCE_DIR}/include/TheCalculater/math/-.cppm
    ${CMAKE_SOURCE_DIR}/include/TheCalculater/math/analytic_expression.cppm
    ${CMAKE_SOURCE_DIR}/include/TheCalculater/math/concepts.cppm
    ${CMAKE_SOURCE_DIR}/include/TheCalculater/math/formatter.cppm
    ${CMAKE_SOURCE_DIR}/include/TheCalculater/math/fraction.cppm
    ${CMAKE_SOURCE_DIR}/include/TheCalculater/math/prime_factorization.cppm
    ${CMAKE_SOURCE_DIR}/include/TheCalculater/settings/-.cppm
    ${CMAKE_SOURCE_DIR}/include/TheCalculater/settings/api.cppm
    ${CMAKE_SOURCE_DIR}/include/TheCalculater/settings/exceptions.cppm
    ${CMAKE_SOURCE_DIR}/include/TheCalculater/settings/value.cppm
    ${CMAKE_SOURCE_DIR}/include/TheCalculater/settings/property.cppm
    ${CMAKE_SOURCE_DIR}/include/TheCalculater/translator/-.cppm
)
# Module Implementation Unit files
target_sources(${PROJECT_NAME}
PRIVATE
    math/analytic_expression.cpp
    math/fraction.cpp
    settings/api.cpp
    settings/math_fraction_calculation_config_impl.cpp
    settings/value.cpp
    translator/-.cpp
    util/exceptions.cpp
    util/json.cpp
    util/thread.cpp
    throwEx.cpp
    libTheCalculaterCommon.cpp
)

target_compile_options(${PROJECT_NAME} PRIVATE -fexceptions -frtti -fexec-charset=UTF-8 -finput-charset=UTF-8 -fno-omit-frame-pointer -g)
if(WIN32)
    target_compile_options(${PROJECT_NAME} PRIVATE -Wa,-mbig-obj)
endif()


target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/third_party/spdlog/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/third_party/CLI11/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/third_party/libbacktrace/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/third_party/json5cpp/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/third_party/jsoncpp/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_BINARY_DIR}/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${BOOST_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE ${JSONCPP_LIBRARY})
target_link_libraries(${PROJECT_NAME} PRIVATE ${BACKTRACE_LIBRARY})

target_compile_definitions(${PROJECT_NAME} PRIVATE BOOST_STACKTRACE_USE_BACKTRACE)
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