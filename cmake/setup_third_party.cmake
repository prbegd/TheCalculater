 # Copyright © 2026 Cai Yaoxing
 # 
 # This file is part of TheCalculater.
 # TheCalculater is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 # TheCalculater is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 # You should have received a copy of the GNU General Public License along with TheCalculater. If not, see <https://www.gnu.org/licenses/>.

message(STATUS "Using conan to install third party packages.")

find_program(THIRDPARTY_CLANG_EXECUTABLE NAMES clang clang.exe)
find_program(THIRDPARTY_CLANGXX_EXECUTABLE NAMES clang++ clang++.exe)
execute_process(COMMAND ${THIRDPARTY_CLANG_EXECUTABLE} -dumpversion OUTPUT_VARIABLE THIRDPARTY_CLANG_VERSION)
string(REGEX MATCH "^[0-9]+" THIRDPARTY_CLANG_VERSION_MAJOR "${THIRDPARTY_CLANG_VERSION}")

if(WIN32)
    find_program(THIRDPARTY_GCC_EXECUTABLE NAMES gcc gcc.exe)
    find_program(THIRDPARTY_GXX_EXECUTABLE NAMES g++ g++.exe)
    execute_process(COMMAND ${THIRDPARTY_GCC_EXECUTABLE} -dumpversion OUTPUT_VARIABLE THIRDPARTY_GCC_VERSION)
    string(REGEX MATCH "^[0-9]+" THIRDPARTY_GCC_VERSION_MAJOR "${THIRDPARTY_GCC_VERSION}")
    configure_file(${CMAKE_SOURCE_DIR}/cmake/conan_profiles/windows.in ${CMAKE_BINARY_DIR}/conan_profiles/windows @ONLY)

    # Since not cross compiling, keep host and build profile the same here.
    set(CONAN_HOST_PROFILE "${CMAKE_BINARY_DIR}/conan_profiles/windows" CACHE STRING "Conan host profile used in third party installation")
    set(CONAN_BUILD_PROFILE "${CMAKE_BINARY_DIR}/conan_profiles/windows" CACHE STRING "Conan build profile used in third party installation")
elseif(UNIX AND APPLE)
    configure_file(${CMAKE_SOURCE_DIR}/cmake/conan_profiles/macos.in ${CMAKE_BINARY_DIR}/conan_profiles/macos @ONLY)
    set(CONAN_HOST_PROFILE "${CMAKE_BINARY_DIR}/conan_profiles/macos" CACHE STRING "Conan host profile used in third party installation")
    set(CONAN_BUILD_PROFILE "${CMAKE_BINARY_DIR}/conan_profiles/macos" CACHE STRING "Conan build profile used in third party installation")
else()
    configure_file(${CMAKE_SOURCE_DIR}/cmake/conan_profiles/linux.in ${CMAKE_BINARY_DIR}/conan_profiles/linux @ONLY)
    set(CONAN_HOST_PROFILE "${CMAKE_BINARY_DIR}/conan_profiles/linux" CACHE STRING "Conan host profile used in third party installation")
    set(CONAN_BUILD_PROFILE "${CMAKE_BINARY_DIR}/conan_profiles/linux" CACHE STRING "Conan build profile used in third party installation")
endif()
if(NOT SKIP_DEPLOYMENT)
    set(CONAN_INSTALL_ARGS "--build=missing;--build=boost*;--deployer=${CMAKE_SOURCE_DIR}/cmake/shared_deployer;--deployer-folder=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}" CACHE STRING "Command line arguments for conan install")
endif()

function(installConanHooks)
    set(RECIPE_PATCHES_DIR "${CMAKE_SOURCE_DIR}/cmake/recipe_patches")

    find_program(conanExecutable NAMES conan conan.exe)
    if(NOT conanExecutable)
        message(FATAL_ERROR "Could not find conan executable!")
    endif()
    execute_process(COMMAND ${conanExecutable} config home OUTPUT_VARIABLE conanHome)
    string(STRIP ${conanHome} conanHome)
    set(destDir "${conanHome}/extensions/hooks")
    file(GLOB_RECURSE inFiles "${CMAKE_SOURCE_DIR}/cmake/conan_hooks/*.in")
    foreach(inFile ${inFiles})
        file(RELATIVE_PATH relativePath "${CMAKE_SOURCE_DIR}/cmake/conan_hooks" ${inFile})
        string(REGEX REPLACE "\\.in$" "" outFilename "${relativePath}")
        set(outFile "${destDir}/${outFilename}")
        get_filename_component(outDir "${outFile}" DIRECTORY)
        file(MAKE_DIRECTORY "${outDir}")
        configure_file("${inFile}" "${outFile}" @ONLY)
    endforeach()

    file(GLOB_RECURSE pyFiles "${CMAKE_SOURCE_DIR}/cmake/conan_hooks/*.py")
    foreach(pyFile ${pyFiles})
        file(RELATIVE_PATH copyFilename "${CMAKE_SOURCE_DIR}/cmake/conan_hooks" ${pyFile})
        set(copyFile "${destDir}/${copyFilename}")
        get_filename_component(copyDir "${copyFile}" DIRECTORY)
        file(MAKE_DIRECTORY "${copyDir}")
        configure_file("${pyFile}" "${copyFile}" COPYONLY)
    endforeach()
    
    message(STATUS "Installed conan hooks to '${destDir}'")
endfunction()
installConanHooks()
# TODO: Uninstall these hooks

include("${CMAKE_SOURCE_DIR}/cmake/conan_provider.cmake")