if(WIN32)
    qt_add_executable(TheCalculater
        MANUAL_FINALIZATION
        "${CMAKE_SOURCE_DIR}/resources.rc"
    )
else()
    qt_add_executable(TheCalculater
        MANUAL_FINALIZATION
    )
endif()

target_sources(TheCalculater
PUBLIC FILE_SET CXX_MODULES FILES
    prbegd/thecalculater/debugging.cppm
)
target_sources(TheCalculater PRIVATE
    main.cpp
    ui/mainwindow.cpp
    ui/mainwindow.h
    ui/mainwindow.ui
)

target_compile_options(TheCalculater PRIVATE -fexceptions -frtti -fexec-charset=UTF-8 -finput-charset=UTF-8 -fno-omit-frame-pointer -g)
if(WIN32)
    target_compile_options(TheCalculater PRIVATE -Wa,-mbig-obj -fseh-exceptions)
endif()

target_link_libraries(TheCalculater PRIVATE TheCalculaterCore ThirdPartyCore ThirdPartyExtra)

if(POSIX)
    target_link_libraries(TheCalculater PRIVATE ${CMAKE_DL_LIBS} Threads::Threads)
endif()

target_include_directories(TheCalculater PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_BINARY_DIR}/include
    ${CMAKE_SOURCE_DIR}/thirdparty/json5cpp/include
)

set_target_properties(TheCalculater PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

qt_finalize_executable(TheCalculater)

function(deployDependencies)
    getConanFirstBinaryPackageId("qt/6.10.1" QT_CONAN_PACKAGE_ID)
    getConanPackageFolder(${QT_CONAN_PACKAGE_ID} QT_CONAN_BINARY_PACKAGE_FOLDER)
    if(WIN32)
        add_custom_command(TARGET TheCalculater POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E echo "Deploying Qt libraries and plugins..."
            COMMAND ${QT_CONAN_BINARY_PACKAGE_FOLDER}/bin/windeployqt "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/TheCalculater.exe" --no-translations --no-network  --no-system-dxc-compiler --no-system-d3d-compiler --no-opengl-sw  --verbose=0
        ) 
    elseif(APPLE)
        add_custom_command(TARGET TheCalculater POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E echo "Deploying Qt libraries and plugins..."
            COMMAND ${QT_CONAN_BINARY_PACKAGE_FOLDER}/bin/macdeployqt "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/TheCalculater.app" -verbose=0
        ) 
    else()
        add_custom_command(TARGET TheCalculater POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E echo "Deploying Qt libraries and plugins..."
            COMMAND python ${CMAKE_SOURCE_DIR}/cmake/linuxdeployqt6.py "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/TheCalculater" -no-translations -verbose=0 -qtdir ${QT_CONAN_BINARY_PACKAGE_FOLDER}
        ) 
    endif()
    add_custom_command(TARGET TheCalculater POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/README.adoc" "${CMAKE_SOURCE_DIR}/LICENSE" "${CMAKE_SOURCE_DIR}/CHANGELOG.adoc" "${CMAKE_SOURCE_DIR}/NOTICE.adoc" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
    ) 
endfunction()
if(NOT SKIP_DEPLOYMENT)
    deployDependencies()
endif()