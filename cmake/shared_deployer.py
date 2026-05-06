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