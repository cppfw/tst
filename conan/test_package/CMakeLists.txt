cmake_minimum_required(VERSION 3.15)
project(PackageTest CXX)

# set(CMAKE_VERBOSE_MAKEFILE on)

find_package(tst CONFIG REQUIRED)

add_executable(example example.cpp)
target_link_libraries(example tst::tst)
target_link_options(example -pthread)
