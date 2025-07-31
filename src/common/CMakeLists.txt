project(TheCalculaterCommon)

file(GLOB_RECURSE COMMON_SOURCES
"*.cpp"
)

add_library(${PROJECT_NAME} SHARED ${COMMON_SOURCES})

target_compile_definitions(${PROJECT_NAME} PRIVATE THECALCULATERCOMMON_EXPORTS)

target_compile_options(${PROJECT_NAME} PRIVATE -fexceptions -frtti -fexec-charset=UTF-8 -finput-charset=UTF-8 -fno-omit-frame-pointer -g)
if(MINGW)
    target_compile_options(${PROJECT_NAME} PRIVATE -Wa,-mbig-obj)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -rdynamic)
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
target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Core)

if(APPLE) # clang
    target_compile_definitions(${PROJECT_NAME} PRIVATE _GNU_SOURCE)
endif()
