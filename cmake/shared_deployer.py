 # Copyright © 2026 Cai Yaoxing
 # 
 # This file is part of TheCalculater.
 # TheCalculater is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 # TheCalculater is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 # You should have received a copy of the GNU General Public License along with TheCalculater. If not, see <https://www.gnu.org/licenses/>.

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