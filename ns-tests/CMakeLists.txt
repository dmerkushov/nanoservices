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

set(Headers
		testing.h
		testBase64.h
		testExceptionThrowThrough.h)

include_directories(${ns_skeleton_SOURCE_DIR}/include)
# Header-only MSGPACK realisation
include_directories(${nanoservices_SOURCE_DIR}/third-party/msgpack-c/include)

add_executable(ns-tests ${Sources})

if (${GLOBAL_BUILD})
	message("ns-tests: Will link ns-tests with ${ns_skeleton_BINARY_DIR}/libns-skeleton.so")
	target_link_libraries(ns-tests ns-skeleton)
	
	# This for use pretty json in json-*.txt	
	set(JSON_FILE json.txt)
	set(SET_UP_CMD "${ns_configurator_BINARY_DIR}/ns-configurator -c -p '/myService' -a \"$(cat\ ${JSON_FILE})\"")
	set(TEAR_DOWN_CMD "${ns_configurator_BINARY_DIR}/ns-configurator -d -p '/myService'")

	add_custom_command(TARGET ns-tests
                        POST_BUILD
                        COMMAND bash -c "${SET_UP_CMD}"
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
			VERBATIM
                        )	
	add_custom_command(TARGET ns-tests POST_BUILD COMMAND ${CMAKE_CURRENT_BINARY_DIR}/ns-tests)
	add_custom_command(TARGET ns-tests POST_BUILD COMMAND bash -c "${TEAR_DOWN_CMD}" VERBATIM)
else ()
	message(WARNING "ns-tests: Not a global build, so no libns-skeleton.so to link with and run tests against")
endif ()
