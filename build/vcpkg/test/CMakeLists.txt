cmake_minimum_required(VERSION 3.10)

set(CMAKE_TOOLCHAIN_FILE $ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)

project(test)

find_package(utki CONFIG REQUIRED)
find_package(clargs CONFIG REQUIRED)
find_package(opros CONFIG REQUIRED)
find_package(nitki CONFIG REQUIRED)
find_package(tst CONFIG REQUIRED)

add_executable(test main.cpp)

target_link_libraries(test PRIVATE tst::tst)
