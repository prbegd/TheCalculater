 # Copyright © 2026 Cai Yaoxing
 # 
 # This file is part of TheCalculater.
 # TheCalculater is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 # TheCalculater is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 # You should have received a copy of the GNU General Public License along with TheCalculater. If not, see <https://www.gnu.org/licenses/>.
project(tests)

add_executable(${PROJECT_NAME} test_main.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/thirdparty/json5cpp/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_BINARY_DIR}/include)

target_link_libraries(${PROJECT_NAME} PRIVATE TheCalculaterCore ThirdPartyCore ThirdPartyExtra Catch2::Catch2WithMain)

target_compile_definitions(${PROJECT_NAME} PRIVATE BOOST_STACKTRACE_USE_BACKTRACE)
if(APPLE) # clang
    target_compile_definitions(${PROJECT_NAME} PRIVATE _GNU_SOURCE)
endif()