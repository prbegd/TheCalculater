 # Copyright © 2026 Cai Yaoxing
 # 
 # This file is part of TheCalculater.
 # TheCalculater is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 # TheCalculater is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 # You should have received a copy of the GNU General Public License along with TheCalculater. If not, see <https://www.gnu.org/licenses/>.
add_library(ThirdPartyCore SHARED)
target_sources(ThirdPartyCore PUBLIC FILE_SET CXX_MODULES FILES
    thirdparty/core-boost.cppm
    thirdparty/core-cli11.cppm
    thirdparty/core-cstd.cppm
    thirdparty/core-json5cpp.cppm
    thirdparty/core-jsoncpp.cppm
    thirdparty/core-posixapi.cppm
    thirdparty/core-spdlog.cppm
    thirdparty/core-winapi.cppm
    thirdparty/core.cppm
)
target_sources(ThirdPartyCore PRIVATE
    thirdparty/spdlog.cpp
    thirdparty/core-winapi.cpp
)

target_include_directories(ThirdPartyCore PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_include_directories(ThirdPartyCore PUBLIC ${CMAKE_SOURCE_DIR}/thirdparty/json5cpp/include)

target_link_libraries(ThirdPartyCore PUBLIC libbacktrace::libbacktrace)
target_link_libraries(ThirdPartyCore PUBLIC Boost::headers)
target_link_libraries(ThirdPartyCore PUBLIC JsonCpp::JsonCpp)
target_link_libraries(ThirdPartyCore PUBLIC spdlog::spdlog_header_only)
target_link_libraries(ThirdPartyCore PUBLIC fmt::fmt)
target_link_libraries(ThirdPartyCore PUBLIC CLI11::CLI11)

# To prevent {fmt} from calling #pragma optimize in modules
target_compile_definitions(ThirdPartyCore PUBLIC __OPTIMIZE__) 
if(APPLE) # clang
    target_compile_definitions(ThirdPartyCore PUBLIC _GNU_SOURCE)
endif()
# Windows 10 1809 "Redstone 5" and later
# According to https://doc.qt.io/qt-6/windows.html
target_compile_definitions(ThirdPartyCore PUBLIC _WIN32_WINNT=0x0A00)
target_compile_definitions(ThirdPartyCore PUBLIC NTDDI_VERSION=0x0A000006)

add_library(ThirdPartyExtra STATIC)

target_sources(ThirdPartyExtra PUBLIC FILE_SET CXX_MODULES FILES
    thirdparty/extra-qt.cppm
    thirdparty/extra.cppm
)
target_link_libraries(ThirdPartyExtra PUBLIC qt::qt)
