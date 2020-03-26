cmake_minimum_required(VERSION 3.0)
project(nanoservices)

option(ZOO "make with zoo config" ON)
include(CMakeDependentOption)
cmake_dependent_option(FILE "make with file config" OFF "ZOO" ON)

set(GLOBAL_BUILD true)

add_subdirectory(ns-skeleton)
add_subdirectory(ns-tests)
add_subdirectory(misc)


