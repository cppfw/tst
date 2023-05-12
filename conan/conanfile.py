import os
from conan import ConanFile
from conan.tools.scm import Git
from conan.tools.files import load, update_conandata, copy
from conan.tools.layout import basic_layout

class TstConan(ConanFile):
	name = "tst"
	license = "MIT"
	author = "Ivan Gagis <igagis@gmail.com>"
	url = "http://github.com/cppfw/" + name
	description = "xUnit-like testig framework for C++"
	topics = ("C++", "cross-platform")
	settings = "os", "compiler", "build_type", "arch"
	package_type = "library"
	options = {"shared": [True, False], "fPIC": [True, False]}
	default_options = {"shared": False, "fPIC": True}
	generators = "AutotoolsDeps" # this will set CXXFLAGS etc. env vars

	def requirements(self):
		self.requires("utki/[>=1.1.202]@cppfw/main", transitive_headers=True)
		self.requires("clargs/[>=0.0.0]@cppfw/main", transitive_headers=True)
		self.requires("nitki/[>=0.0.0]@cppfw/main", transitive_headers=True)

	def config_options(self):
		if self.settings.os == "Windows":
			del self.options.fPIC

	# save commit and remote URL to conandata.yml for packaging
	def export(self):
		git = Git(self, self.recipe_folder)
		scm_url = git.get_remote_url()
		scm_commit = git.get_commit()
		update_conandata(self, {"sources": {"commit": scm_commit, "url": scm_url}})

	def source(self):
		git = Git(self)
		sources = self.conan_data["sources"]
		# shallow fetch commit
		git.fetch_commit(url=sources["url"], commit=sources['commit'])
		# shallow clone submodules
		git.run("submodule update --init --remote --depth 1")

	def build(self):
		self.run("make lint=off")
		self.run("make lint=off test")

	def package(self):
		src_dir = os.path.join(self.build_folder, "src")
		src_rel_dir = os.path.join(self.build_folder, "src/out/rel")
		dst_include_dir = os.path.join(self.package_folder, "include")
		dst_lib_dir = os.path.join(self.package_folder, "lib")
		dst_bin_dir = os.path.join(self.package_folder, "bin")
		
		copy(conanfile=self, pattern="*.h",                    dst=dst_include_dir, src=src_dir,     keep_path=True)
		copy(conanfile=self, pattern="*.hpp",                  dst=dst_include_dir, src=src_dir,     keep_path=True)

		if self.options.shared:
			copy(conanfile=self, pattern="*" + self.name + ".lib", dst=dst_lib_dir,     src="",          keep_path=False)
			copy(conanfile=self, pattern="*.dll",                  dst=dst_bin_dir,     src=src_rel_dir, keep_path=False)
			copy(conanfile=self, pattern="*.so",                   dst=dst_lib_dir,     src=src_rel_dir, keep_path=False)
			copy(conanfile=self, pattern="*.so.*",                 dst=dst_lib_dir,     src=src_rel_dir, keep_path=False)
			copy(conanfile=self, pattern="*.dylib",                dst=dst_lib_dir,     src=src_rel_dir, keep_path=False)
		else:
			copy(conanfile=self, pattern="*" + self.name + ".lib", dst=dst_lib_dir,     src="",          keep_path=False)
			copy(conanfile=self, pattern="*.a",                    dst=dst_lib_dir,     src=src_rel_dir, keep_path=False)

	def package_info(self):
		self.cpp_info.libs = [self.name]

	def package_id(self):

		# change package id only when minor or major version changes, i.e. when ABI breaks
		self.info.requires.minor_mode()






from conans import ConanFile, CMake, tools

class TstConan(ConanFile):
	name = "tst"
	version = "$(version)"
	license = "MIT"
	author = "Ivan Gagis <igagis@gmail.com>"
	url = "http://github.com/cppfw/" + name
	description = "xUnit-like testig framework for C++"
	topics = ("C++", "cross-platform")
	settings = "os", "compiler", "build_type", "arch"
	options = {"shared": [True, False], "fPIC": [True, False]}
	default_options = {"shared": False, "fPIC": True}
	requires = "utki/[>=0.0.0]@cppfw/main", "clargs/[>=0.0.0]@cppfw/main", "nitki/[>=0.0.0]@cppfw/main"
	generators = "make"
	scm = {"type": "git", "url": "auto", "revision": "auto"}

	def config_options(self):
		if self.settings.os == "Windows":
			del self.options.fPIC

	# we use the 'scm' attribute, so the source() method is not needed
#	def source(self):
#		self.run("git clone https://github.com/cppfw/" + self.name + ".git")
#		self.run("cd " + self.name + " && git checkout " + self.version)

		# This small hack might be useful to guarantee proper /MT /MD linkage
		# in MSVC if the packaged project doesn't have variables to set it
		# properly
# 		tools.replace_in_file("hello/CMakeLists.txt", "PROJECT(HelloWorld)",
# 							'''PROJECT(HelloWorld)
# include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
# conan_basic_setup()''')

	def build(self):
		self.run("CONANBUILDINFO_DIR=$(pwd)/ make lint=off")
		self.run("CONANBUILDINFO_DIR=$(pwd)/ make lint=off test")

	def package(self):
		self.copy("*.h", dst="include", src="src")
		self.copy("*.hpp", dst="include", src="src")
		self.copy("*" + self.name + ".lib", dst="lib", keep_path=False)
		self.copy("*.dll", dst="bin", src=  "src/out/rel", keep_path=False)
		self.copy("*.so", dst="lib", src=   "src/out/rel", keep_path=False)
		self.copy("*.so.*", dst="lib", src= "src/out/rel", keep_path=False)
		self.copy("*.dylib", dst="lib", src="src/out/rel", keep_path=False)
		self.copy("*.a", dst="lib", src=    "src/out/rel", keep_path=False)

	def package_info(self):
		self.cpp_info.libs = [self.name]

	# change package id only when minor or major version changes, i.e. when ABI breaks
	def package_id(self):
		self.info.requires.minor_mode()
