 # Copyright © 2026 Cai Yaoxing
 # 
 # This file is part of TheCalculater.
 # TheCalculater is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 # TheCalculater is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 # You should have received a copy of the GNU General Public License along with TheCalculater. If not, see <https://www.gnu.org/licenses/>.

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