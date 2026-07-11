from conan import ConanFile
import os

_script = os.path.basename(__file__)

# TODO: the most awkward hack i've ever seen
def pre_source(conanfile: ConanFile):
    if str(conanfile.name) != "libbacktrace":
        return
    try:
        conanfile.conan_data["sources"][conanfile.version]["url"] = "https://github.com/ianlancetaylor/libbacktrace/archive/0ec32ff4354df040785c73fa19902828f8290c39.tar.gz"
        del conanfile.conan_data["sources"][conanfile.version]["sha256"]
        del conanfile.conan_data["patches"]
        print(f"{_script}: Changed libbacktrace download source url.")
    except Exception:
        print(f"{_script}: Error while changing libbacktrace download source url.")
        raise