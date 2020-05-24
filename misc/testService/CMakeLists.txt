cmake_minimum_required(VERSION 3.0)
project(test_service)

set(Sources 
	testMethod.cpp
	main.cpp)

set(Headers
	testMethod.h)

add_executable(testService ${Headers} ${Sources})

find_package(Nanoservices REQUIRED)

include_directories(${LIBNSSKELETON_INCLUDE_DIRS})
target_link_libraries(testService ${LIBNSSKELETON_LIBRARIES})
