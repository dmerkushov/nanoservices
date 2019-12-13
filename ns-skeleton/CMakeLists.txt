cmake_minimum_required(VERSION 3.0)
project(ns-skeleton)

if(${GLOBAL_BUILD})
	message("Preparing ns-skeleton build as a part of global nanoservices build")
else()
	message("Preparing a standalone ns-skeleton build")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")

file(GLOB_RECURSE SOURCES src/*.cpp)
include_directories(include)

add_library(ns-skeleton SHARED ${SOURCES})
