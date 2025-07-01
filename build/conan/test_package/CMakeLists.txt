cmake_minimum_required(VERSION 3.15)
project(PackageTest CXX)

# set(CMAKE_VERBOSE_MAKEFILE on)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads)

find_package(tst CONFIG REQUIRED)

add_executable(example example.cpp)
target_link_libraries(example tst::tst Threads::Threads)
