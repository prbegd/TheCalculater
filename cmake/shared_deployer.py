# Copyright © 2025 Cai Yaoxing
# SPDX-License-Identifier: GPL-3.0-only
# This file is part of TheCalculater.
# See the file LICENSE in the project root or go to 
# <https://www.gnu.org/licenses/gpl-3.0.html> for detailed license information.

import os
from conan.tools.files import copy

def deploy(graph, output_folder, **kwargs):
    for dep in graph.root.conanfile.dependencies.values():
        # We'll deploy Qt separately using windeployqt, macdeployqt and linuxdeployqt6.py, so we skip it here
        if dep.ref.name == "qt":
            continue
        libdirs = dep.cpp_info.libdirs
        bindirs = dep.cpp_info.bindirs

        for d in libdirs:
            if os.path.isdir(d):
                copy(None, "*.so*", src=d, dst=output_folder)
                copy(None, "*.dylib*", src=d, dst=output_folder)

        for d in bindirs:
            if os.path.isdir(d):
                copy(None, "*.dll", src=d, dst=output_folder)