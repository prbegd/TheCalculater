# Copyright © 2025 Cai Yaoxing
# SPDX-License-Identifier: GPL-3.0-only
# This file is part of TheCalculater.
# See the file LICENSE in the project root or go to 
# <https://www.gnu.org/licenses/gpl-3.0.html> for detailed license information.

# ! HelperPipeReader is only be built when on Windows.

project(HelperPipeReader VERSION 0.0.1)

add_executable(${PROJECT_NAME}
    "./main.cpp"
)

target_compile_options(${PROJECT_NAME} PRIVATE -fexec-charset=UTF-8 -finput-charset=UTF-8)

set_target_properties(${PROJECT_NAME} PROPERTIES
    WIN32_EXECUTABLE FALSE
)