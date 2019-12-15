cmake_minimum_required(VERSION 3.0)
project(ns_tests)

set(CMAKE_CXX_STANDARD 11)

if (${GLOBAL_BUILD})
	message("Preparing ns-tests build as a part of global nanoservices build")
else ()
	message(WARNING "ns-tests: Should not build ns-tests outside global nanoservices build")
endif ()

set(Sources
		main.cpp
		testing.cpp testing.h
		testBase64.cpp testBase64.h
		testExceptionThrowThrough.cpp testExceptionThrowThrough.h
		testBrokenWrite.cpp testBrokenWrite.h)

message("Sources: ${Sources}")

include_directories(../ns-skeleton/include)

add_executable(ns-tests ${Sources})

if (${GLOBAL_BUILD})
	message("ns-tests: Will link ns-tests with ${ns_skeleton_BINARY_DIR}/libns-skeleton.so")
	target_link_libraries(ns-tests ns-skeleton)
	add_custom_command(TARGET ns-tests POST_BUILD COMMAND ${ns_tests_BINARY_DIR}/ns-tests)
else ()
	message(WARNING "ns-tests: Not a global build, so no libns-skeleton.so to link with and run tests against")
endif ()
