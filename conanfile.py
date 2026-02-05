from conan import ConanFile
from conan.tools.build import check_min_cppstd
from conan.tools.files import load, copy, collect_libs
from conan.tools.cmake import cmake_layout, CMake, CMakeToolchain
import re
import os


class PackageConan(ConanFile):
    # @+ START USER META CONFIG
    license = "Apache 2.0"
    description = ""
    topics = ("")
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False],
               "fPIC": [True, False]}
    default_options = {"shared": True,
                       "fPIC": True}
    default_user = "Hahn-Schickard"
    # @- END USER META CONFIG
    exports = [
        "CMakeLists.txt",
        "conanfile.py"
    ]
    exports_sources = [
        "cmake*",
        "includes*",
        "sources*",
        "CMakeLists.txt",
        "conanfile.py",
        # @+ START USER EXPORTS
        "config*",
        # @- END USER EXPORTS
    ]
    generators = "CMakeDeps"
    package_type = "library"
    short_paths = True

    @property
    def cwd(self):
        return os.path.dirname(os.path.realpath(__file__))

    @property
    def full_name(self):
        content = load(self, path=os.path.join(
            self.recipe_folder, 'CMakeLists.txt'))
        return re.search('set\(THIS (.*)\)', content).group(1).strip()

    def set_name(self):
        self.name = self.full_name.lower()

    def validate(self):
        if self.settings.compiler.get_safe("cppstd"):
            check_min_cppstd(self, "17")

    def requirements(self):
        # @+ START USER REQUIREMENTS
        self.requires("pugixml/1.15")
        self.requires("date/3.0.1")
        self.requires("variant_visitor/[~0.1]@hahn-schickard/stable",
                      headers=True,
                      transitive_headers=True
        )
        self.requires("hscul/[~0.3]@hahn-schickard/stable",
                      headers=True,
                      libs=True,
                      transitive_headers=True,
                      transitive_libs=True
        )  
        # @- END USER REQUIREMENTS

    def build_requirements(self):
        self.test_requires("gtest/[~1.16]")
        # @+ START USER BUILD REQUIREMENTS
        # @- END USER BUILD REQUIREMENTS

    def configure(self):
        # @+ START USER REQUIREMENTS OPTION CONFIGURATION
        self.options["gtest/*"].shared = True
        self.options["date/*"].header_only = True
        self.options["date/*"].tz_db = "system"
        # @- END USER REQUIREMENTS OPTION CONFIGURATION

    def layout(self):
        cmake_layout(self)

    def config_options(self):
        if self.settings.os == 'Windows':
            del self.options.fPIC

    def generate(self):
        tc = CMakeToolchain(self)
        tc.user_presets_path = False
        tc.variables['STATIC_CODE_ANALYSIS'] = False
        tc.variables['RUN_TESTS'] = False
        tc.variables['COVERAGE_TRACKING'] = False
        tc.variables['CMAKE_CONAN'] = False
        # @+ START USER CMAKE OPTIONS
        # @- END USER CMAKE OPTIONS
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()
        copy(self, pattern='LICENSE', dst='licenses', src=self.cwd)
        copy(self, pattern='NOTICE', dst='licenses', src=self.cwd)
        copy(self, pattern='AUTHORS', dst='licenses', src=self.cwd)

    def package_info(self):
        self.cpp_info.libs = collect_libs(self)
        self.cpp_info.set_property("cmake_find_mode", "both")
        # @+ START USER DEFINES
        # @- END USER DEFINES
        self.cpp_info.set_property("cmake_file_name", self.full_name)
        cmake_target_name = self.full_name + "::" + self.full_name
        self.cpp_info.set_property("cmake_target_name", cmake_target_name)
