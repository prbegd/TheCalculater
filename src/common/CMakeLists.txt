 # Copyright © 2026 Cai Yaoxing
 # 
 # This file is part of TheCalculater.
 # TheCalculater is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 # TheCalculater is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 # You should have received a copy of the GNU General Public License along with TheCalculater. If not, see <https://www.gnu.org/licenses/>.

add_library(TheCalculaterCommon SHARED)
# Module Interface Unit files
target_sources(TheCalculaterCommon
PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${CMAKE_SOURCE_DIR} FILES
    ${CMAKE_SOURCE_DIR}/include/thecalculater/throwEx.cppm  
    ${CMAKE_SOURCE_DIR}/include/thecalculater/libTheCalculaterCommon.cppm
    ${CMAKE_SOURCE_DIR}/include/thecalculater/util/-.cppm
    ${CMAKE_SOURCE_DIR}/include/thecalculater/util/exceptions.cppm
    ${CMAKE_SOURCE_DIR}/include/thecalculater/util/hasher.cppm
    ${CMAKE_SOURCE_DIR}/include/thecalculater/util/json.cppm
    ${CMAKE_SOURCE_DIR}/include/thecalculater/util/memory.cppm
    ${CMAKE_SOURCE_DIR}/include/thecalculater/util/thread.cppm
    ${CMAKE_SOURCE_DIR}/include/thecalculater/util/io.cppm
    ${CMAKE_SOURCE_DIR}/include/thecalculater/util/constexpr_string.cppm
    ${CMAKE_SOURCE_DIR}/include/thecalculater/util/shortcuts.cppm
    ${CMAKE_SOURCE_DIR}/include/thecalculater/math/-.cppm
    ${CMAKE_SOURCE_DIR}/include/thecalculater/math/analytic_expression.cppm
    ${CMAKE_SOURCE_DIR}/include/thecalculater/math/concepts.cppm
    ${CMAKE_SOURCE_DIR}/include/thecalculater/math/formatter.cppm
    ${CMAKE_SOURCE_DIR}/include/thecalculater/math/fraction.cppm
    ${CMAKE_SOURCE_DIR}/include/thecalculater/math/prime_factorization.cppm
    ${CMAKE_SOURCE_DIR}/include/thecalculater/settings/-.cppm
    ${CMAKE_SOURCE_DIR}/include/thecalculater/settings/api.cppm
    ${CMAKE_SOURCE_DIR}/include/thecalculater/settings/exceptions.cppm
    ${CMAKE_SOURCE_DIR}/include/thecalculater/settings/value.cppm
    ${CMAKE_SOURCE_DIR}/include/thecalculater/settings/property.cppm
    ${CMAKE_SOURCE_DIR}/include/thecalculater/translator/-.cppm
)
# Module Implementation Unit files
target_sources(TheCalculaterCommon
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

target_compile_options(TheCalculaterCommon PRIVATE -fexceptions -frtti -fexec-charset=UTF-8 -finput-charset=UTF-8 -fno-omit-frame-pointer -g)
if(WIN32)
    target_compile_options(TheCalculaterCommon PRIVATE -Wa,-mbig-obj -fseh-exceptions)
    target_link_options(TheCalculaterCommon PRIVATE -Wl,--export-all-symbols)
endif()

target_compile_definitions(TheCalculaterCommon PRIVATE TCAPI_DLLEXPORT)

target_include_directories(TheCalculaterCommon PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_include_directories(TheCalculaterCommon PRIVATE ${CMAKE_BINARY_DIR}/include)

target_link_libraries(TheCalculaterCommon PRIVATE TheCalculaterThirdPartyCore)