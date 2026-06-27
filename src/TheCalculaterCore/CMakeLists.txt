 # Copyright © 2026 Cai Yaoxing
 # 
 # This file is part of TheCalculater.
 # TheCalculater is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 # TheCalculater is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 # You should have received a copy of the GNU General Public License along with TheCalculater. If not, see <https://www.gnu.org/licenses/>.

add_library(TheCalculaterCore SHARED)
# Module Interface Unit files
target_sources(TheCalculaterCore
PUBLIC FILE_SET CXX_MODULES FILES
    throwEx.cppm  
    util/-.cppm
    util/exceptions.cppm
    util/hasher.cppm
    util/json.cppm
    util/memory.cppm
    util/thread.cppm
    util/io.cppm
    util/constexpr_string.cppm
    util/shortcuts.cppm
    math/-.cppm
    math/analytic_expression.cppm
    math/concepts.cppm
    math/formatter.cppm
    math/fraction.cppm
    math/prime_factorization.cppm
    settings/-.cppm
    settings/api.cppm
    settings/exceptions.cppm
    settings/value.cppm
    settings/property.cppm
    translator/-.cppm
)
# Module Implementation Unit files
target_sources(TheCalculaterCore
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
)

target_compile_options(TheCalculaterCore PRIVATE -fexceptions -frtti -fexec-charset=UTF-8 -finput-charset=UTF-8 -fno-omit-frame-pointer -g)
if(WIN32)
    target_compile_options(TheCalculaterCore PRIVATE -Wa,-mbig-obj -fseh-exceptions)
    target_link_options(TheCalculaterCore PRIVATE -Wl,--export-all-symbols)
endif()

target_compile_definitions(TheCalculaterCore PRIVATE TCAPI_DLLEXPORT)

target_include_directories(TheCalculaterCore PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_include_directories(TheCalculaterCore PRIVATE ${CMAKE_BINARY_DIR}/include)

target_link_libraries(TheCalculaterCore PRIVATE ThirdPartyCore)