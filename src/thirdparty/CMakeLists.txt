 # Copyright © 2026 Cai Yaoxing
 # 
 # This file is part of TheCalculater.
 # TheCalculater is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 # TheCalculater is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 # You should have received a copy of the GNU General Public License along with TheCalculater. If not, see <https://www.gnu.org/licenses/>.
add_library(TheCalculaterThirdPartyCore SHARED)
target_sources(TheCalculaterThirdPartyCore PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${CMAKE_SOURCE_DIR} FILES
    ${CMAKE_SOURCE_DIR}/include/thirdparty/core-boost.cppm
    ${CMAKE_SOURCE_DIR}/include/thirdparty/core-cli11.cppm
    ${CMAKE_SOURCE_DIR}/include/thirdparty/core-cstd.cppm
    ${CMAKE_SOURCE_DIR}/include/thirdparty/core-json5cpp.cppm
    ${CMAKE_SOURCE_DIR}/include/thirdparty/core-jsoncpp.cppm
    ${CMAKE_SOURCE_DIR}/include/thirdparty/core-posixapi.cppm
    ${CMAKE_SOURCE_DIR}/include/thirdparty/core-spdlog.cppm
    ${CMAKE_SOURCE_DIR}/include/thirdparty/core-winapi.cppm
    ${CMAKE_SOURCE_DIR}/include/thirdparty/core.cppm
)
target_sources(TheCalculaterThirdPartyCore PRIVATE
    spdlog.cpp
    core-winapi.cpp
)

target_include_directories(TheCalculaterThirdPartyCore PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_include_directories(TheCalculaterThirdPartyCore PUBLIC ${CMAKE_SOURCE_DIR}/thirdparty/json5cpp/include)

target_link_libraries(TheCalculaterThirdPartyCore PUBLIC libbacktrace::libbacktrace)
target_link_libraries(TheCalculaterThirdPartyCore PUBLIC Boost::headers)
target_link_libraries(TheCalculaterThirdPartyCore PUBLIC JsonCpp::JsonCpp)
target_link_libraries(TheCalculaterThirdPartyCore PUBLIC spdlog::spdlog_header_only)
target_link_libraries(TheCalculaterThirdPartyCore PUBLIC fmt::fmt)
target_link_libraries(TheCalculaterThirdPartyCore PUBLIC CLI11::CLI11)

# To prevent {fmt} from calling #pragma optimize in modules
target_compile_definitions(TheCalculaterThirdPartyCore PUBLIC __OPTIMIZE__) 
if(APPLE) # clang
    target_compile_definitions(TheCalculaterThirdPartyCore PUBLIC _GNU_SOURCE)
endif()
# Windows 10 1809 "Redstone 5" and later
# According to https://doc.qt.io/qt-6/windows.html
target_compile_definitions(TheCalculaterThirdPartyCore PUBLIC _WIN32_WINNT=0x0A00)
target_compile_definitions(TheCalculaterThirdPartyCore PUBLIC NTDDI_VERSION=0x0A000006)

add_library(TheCalculaterThirdPartyExtra STATIC)

target_sources(TheCalculaterThirdPartyExtra PUBLIC FILE_SET CXX_MODULES BASE_DIRS ${CMAKE_SOURCE_DIR}/include FILES
    ${CMAKE_SOURCE_DIR}/include/thirdparty/extra-qt.cppm
    ${CMAKE_SOURCE_DIR}/include/thirdparty/extra.cppm
)
target_link_libraries(TheCalculaterThirdPartyExtra PUBLIC qt::qt)
