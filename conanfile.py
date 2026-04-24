from conan import ConanFile
from conan.tools.cmake import cmake_layout

class TheCalculaterConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"

    default_options = {
        "qt/*:with_sqlite3": False,
        "qt/*:with_pq": False,
        "qt/*:with_odbc": False,
        "qt/*:openssl": False,
        "qt/*:shared": True,
        "boost/*:header_only": True,
        "boost/*:with_stacktrace_backtrace": False,
        "boost/*:pch": False,
        "boost/*:zlib": False,
        "boost/*:bzip2": False,
        "catch2/*:shared": False,
        "cli11/*:header_only": False,
        "cli11/*:shared": True,
        "jsoncpp/*:shared": True,
        "spdlog/*:header_only": True,
        "fmt/*:shared": True,
        "libbacktrace/*:shared": True,
    }

    generators = "CMakeConfigDeps"

    def layout(self):
        cmake_layout(self)
        
    def requirements(self):
        self.requires("qt/6.10.1")
        self.requires("boost/1.90.0")
        self.requires("catch2/3.14.0")
        self.requires("cli11/2.6.0")
        self.requires("jsoncpp/1.9.6")
        self.requires("spdlog/1.17.0")
        self.requires("libbacktrace/cci.20240730")

