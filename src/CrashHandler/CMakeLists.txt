# Copyright © 2025 prbegd & TheCalculater contributors
# Licensed under the MIT License. See LICENSE in the project root for license information.
project(CrashHandler VERSION 0.0.1)

file(GLOB_RECURSE CRASH_HANDLER_SOURCES
"./*.cpp"
"./*.ui"
"./*.h"
"./*.hpp"
)

qt_add_executable(${PROJECT_NAME}
    MANUAL_FINALIZATION
    ${CRASH_HANDLER_SOURCES}
)

target_compile_options(${PROJECT_NAME} PRIVATE -fexceptions -frtti -fexec-charset=UTF-8 -finput-charset=UTF-8 -fno-omit-frame-pointer -g)
if(MINGW)
    target_compile_options(${PROJECT_NAME} PRIVATE -Wa,-mbig-obj)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -rdynamic)
endif()

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/third_party/spdlog/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/third_party/CLI11/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/third_party/libbacktrace/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/third_party/json5cpp/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/third_party/jsoncpp/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_BINARY_DIR}/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${BOOST_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE TheCalculaterCommon)
target_link_libraries(${PROJECT_NAME} PRIVATE ${JSONCPP_LIBRARY})
target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Core)

set_target_properties(${PROJECT_NAME} PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(${PROJECT_NAME})
endif()

if(APPLE) # clang
    target_compile_definitions(${PROJECT_NAME} PRIVATE _GNU_SOURCE)
endif()
