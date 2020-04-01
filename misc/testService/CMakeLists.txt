cmake_minimum_required(VERSION 3.0)
project(test_service)

set(Source main.cpp)

add_executable(testService ${Source})

find_package(Nanoservices)

include_directories(${LIBNSSKELETON_INCLUDE_DIRS})
target_link_libraries(testService ${LIBNSSKELETON_LIBRARIES})
