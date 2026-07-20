 # Copyright © 2026 Cai Yaoxing
 # 
 # This file is part of TheCalculater.
 # TheCalculater is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 # TheCalculater is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 # You should have received a copy of the GNU General Public License along with TheCalculater. If not, see <https://www.gnu.org/licenses/>.
from conan import ConanFile
from conan.tools.cmake import cmake_layout

class TheCalculaterConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"

    default_options = {
        "boost/*:bzip2": False,
        "boost/*:header_only": True,
        "boost/*:pch": False,
        "boost/*:with_stacktrace_backtrace": False,
        "boost/*:zlib": False,
        "catch2/*:shared": False,
        "cli11/*:header_only": False,
        "cli11/*:shared": True,
        "fmt/*:shared": True,
        "jsoncpp/*:shared": True,
        "libbacktrace/*:shared": True,
        "qt/*:shared": True,
        "qt/*:with_odbc": False,
        "qt/*:with_pq": False,
        "qt/*:with_sqlite3": False,
        "qt/*:with_brotli": False,
        "qt/*:with_openal": False,
        "qt/*:openssl": False,
        "spdlog/*:header_only": True,
        "gmp/*:disable_assembly": False,
        "gmp/*:enable_fat": True,
    }

    generators = "CMakeConfigDeps"

    requires = (
        "boost/1.91.0",
        "catch2/3.14.0",
        "cli11/2.6.0",
        "jsoncpp/1.9.6",
        "libbacktrace/cci.20240730",
        "qt/6.10.1",
        "spdlog/1.17.0",
        "gmp/6.3.0",
    )

    def layout(self):
        cmake_layout(self)
