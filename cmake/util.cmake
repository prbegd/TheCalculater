# Copyright © 2025 Cai Yaoxing
# SPDX-License-Identifier: GPL-3.0-only
# This file is part of TheCalculater.
# See the file LICENSE in the project root or go to 
# <https://www.gnu.org/licenses/gpl-3.0.html> for detailed license information.

find_program(CONAN_EXECUTABLE NAMES conan conan.exe)
function(getConanFirstBinaryPackageId packageName outPackageId)
    execute_process(COMMAND ${CONAN_EXECUTABLE} list "${packageName}:*" --format=json OUTPUT_VARIABLE PACKAGE_JSON)
    string(JSON rev_len LENGTH "${PACKAGE_JSON}" "Local Cache" "${packageName}" "revisions")
    if(NOT rev_len GREATER 0)
        message(FATAL_ERROR "No revisions found in JSON")
    endif()

    string(JSON rev_key MEMBER "${PACKAGE_JSON}" "Local Cache" "${packageName}" "revisions" 0)

    string(JSON pkg_len LENGTH "${PACKAGE_JSON}" "Local Cache" "${packageName}" "revisions" "${rev_key}" "packages")
    if(NOT pkg_len GREATER 0)
        message(FATAL_ERROR "No packages found under revision ${rev_key}")
    endif()

    string(JSON pkg_id MEMBER "${PACKAGE_JSON}" "Local Cache" "${packageName}" "revisions" "${rev_key}" "packages" 0)

    string(STRIP "${packageName}#${rev_key}:${pkg_id}" FULL_PKG_ID)
    set(${outPackageId} ${FULL_PKG_ID} PARENT_SCOPE)
endfunction()
function(getConanPackageFolder package outFolder)
    execute_process(COMMAND "${CONAN_EXECUTABLE}" "cache" "path" "${package}" OUTPUT_VARIABLE PACKAGE_FOLDER)
    set(${outFolder} "${PACKAGE_FOLDER}" PARENT_SCOPE)
endfunction()