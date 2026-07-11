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
    prbegd/thecalculater/math-analytic_expression.cppm
    prbegd/thecalculater/math-concepts.cppm
    prbegd/thecalculater/math-formatter.cppm
    prbegd/thecalculater/math-fraction.cppm
    prbegd/thecalculater/math-prime_factorization.cppm
    prbegd/thecalculater/math.cppm
    prbegd/thecalculater/settings-api.cppm
    prbegd/thecalculater/settings-exceptions.cppm
    prbegd/thecalculater/settings-property.cppm
    prbegd/thecalculater/settings-value.cppm
    prbegd/thecalculater/settings.cppm
    prbegd/thecalculater/translator.cppm
    prbegd/thecalculater/util-constexpr_string.cppm
    prbegd/thecalculater/util-exceptions.cppm
    prbegd/thecalculater/util-hasher.cppm
    prbegd/thecalculater/util-io.cppm
    prbegd/thecalculater/util-json.cppm
    prbegd/thecalculater/util-memory.cppm
    prbegd/thecalculater/util-shortcuts.cppm
    prbegd/thecalculater/util-thread.cppm
    prbegd/thecalculater/util.cppm
)
# Module Implementation Unit files
target_sources(TheCalculaterCore
PRIVATE
    prbegd/thecalculater/math-analytic_expression.cpp
    prbegd/thecalculater/math-fraction_calculation_config_impl.cpp
    prbegd/thecalculater/math-fraction.cpp
    prbegd/thecalculater/settings-api.cpp
    prbegd/thecalculater/settings-value.cpp
    prbegd/thecalculater/translator.cpp
    prbegd/thecalculater/util-exceptions.cpp
    prbegd/thecalculater/util-json.cpp
    prbegd/thecalculater/util-thread.cpp
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