cmake_minimum_required(VERSION 3.0)
project(ns_shutdown)

set(Sources main.cpp)

message("Sources: ${Sources}")

include_directories(${ns_skeleton_SOURCE_DIR}/include)

add_executable(ns-shutdown ${Sources})

if (${GLOBAL_BUILD})
	message("ns-shutdown: Will link ns-shutdown with ${ns_skeleton_BINARY_DIR}/libns-skeleton.so")
	target_link_libraries(ns-shutdown ns-skeleton)
endif()

install(TARGETS ns-shutdown
        RUNTIME DESTINATION bin
        COMPONENT misc)


