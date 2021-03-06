cmake_minimum_required(VERSION 3.0)
project(ns_skeleton)

set(CMAKE_CXX_STANDARD 11)

if (${GLOBAL_BUILD})
	message("Preparing ns-skeleton build as a part of global nanoservices build")
else ()
	message("Preparing a standalone ns-skeleton build")
endif ()

set(Sources
		src/NsException.cpp
		src/NsMonitoring.cpp
		src/NsRpcExecutor.cpp
		src/NsRpcRequest.cpp
		src/NsRpcResponse.cpp
		src/NsRpcResponseError.cpp
		src/NsSerializer.cpp
		src/NsSkelConfiguration.cpp
		src/NsSkeleton.cpp
		src/NsSkelJson.cpp
		src/NsSkelRpcHttpServer.cpp
		src/NsSkelRpcRegistry.cpp
		src/NsSkelRpcReplier.cpp
		src/NsSkelRpcReplierInterface.cpp
		src/NsSkelRpcServer.cpp
		src/NsSkelRpcService.cpp
		src/NsSkelUtils.cpp
		src/NsUtils.cpp
		src/ConfigValidator.cpp
		src/CVInvalidType.cpp
		src/CVNoNodeData.cpp
		src/CVTooManyChildren.cpp
		src/CVTooLongData.cpp
		src/NsCmdLineParameters.cpp
		src/NsSkelLoopWorkerInterface.cpp)

if(FILE)
	set(Sources ${Sources}
		src/file_config/FileConfig.cpp)
elseif(ZOO)
	set(Sources ${Sources}
		src/zoo_config/ZooConfig.cpp)
else()
	message(FATAL_ERROR "Only one of FILE or ZOO must be set.")
endif(FILE)

message("Sources: ${Sources}")

set(Headers
		include/exchange/logging/LogArgs.h
		include/exchange/NsVoidArgs.h
		include/exchange/NsVoidResult.h
		include/sockets/socketstream.h
		include/NsException.h
		include/NsMonitoring.h
		include/NsRpcExecutor.h
		include/NsRpcRequest.h
		include/NsRpcResponse.h
		include/NsRpcResponseError.h
		include/NsSerializer.h
		include/NsSkelConfigException.h
		include/NsSkelConfigGetParamException.h
		include/NsSkelConfiguration.h
		include/NsSkeleton.h
		include/NsSkelException.h
		include/NsSkelFinalizeException.h
		include/NsSkelInitException.h
		include/NsSkelJson.h
		include/NsSkelJsonException.h
		include/NsSkelRpcException.h
		include/NsSkelRpcHttpServer.h
		include/NsSkelRpcRegException.h
		include/NsSkelRpcRegistry.h
		include/NsSkelRpcReplier.h
		include/NsSkelRpcReplierInterface.h
		include/NsSkelRpcReplyException.h
		include/NsSkelRpcServer.h
		include/NsSkelRpcService.h
		include/NsSkelStartException.h
		include/NsSkelUtils.h
		include/NsUtils.h
		include/ConfigValidator.h
		include/CVInvalidType.h
		include/CVNoNodeData.h
		include/CVTooManyChildren.h
		include/CVTooLongData.h
		include/NsCmdLineParameters.h
		include/NsSkelLoopWorkerInterface.h)

if(FILE)
	set(Headers ${Headers}
		include/file_config/FileConfig.h)
elseif(ZOO)
	set(Headers ${Headers}
		include/zoo_config/ZooConfig.h)
else()
	message(FATAL_ERROR "Only one of FILE or ZOO must be set.")
endif(FILE)

include_directories(include)
# Header-only HTTP/HTTPS server
include_directories(${nanoservices_SOURCE_DIR}/third-party/cpp-httplib)
# Header-only MSGPACK realisation
include_directories(${nanoservices_SOURCE_DIR}/third-party/msgpack-c/include)

if(FILE)
	include_directories(include/file_config)
elseif(ZOO)
	include_directories(include/zoo_config)
	include_directories(/usr/include/zookeeper)
	set(CONFIG_LIBS zookeeper_mt)
else()
	message(FATAL_ERROR "Only one of FILE or ZOO must be set.")
endif(FILE)

if(FILE)
	add_definitions(-DURI="file:///etc/nanoservices/global.conf.json")
	add_definitions(-DHEAD="/")	
elseif(ZOO)
	# Connection string to zookeeper instance
	add_definitions(-DURI="zoo://127.0.0.1:2181")
	add_definitions(-DHEAD="")
	# Prefix for root config node in zookeeper
	add_definitions(-DPREFIX="/ru/cniiag/webmaps")
else()
	message(FATAL_ERROR "Only one of FILE or ZOO must be set.")
endif(FILE)

add_library(ns-skeleton SHARED ${Sources} ${Headers})

find_package(Threads)
target_link_libraries(ns-skeleton ${CMAKE_THREAD_LIBS_INIT} ${CONFIG_LIBS})

# Documentation generation: make docs
find_package(Doxygen)
if (DOXYGEN_FOUND)
	#Add target to build documents
	set(DOXYGEN_INPUT ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile)
	#set the output directory of the documentation
	set(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/docs)
	# sanity check...
	message("Doxygen Output ${DOXYGEN_OUTPUT_DIR}")

	# create the output directory where the documentation will live
	file(MAKE_DIRECTORY ${DOXYGEN_OUTPUT_DIR})
	# configure our Doxygen configuration file. This will be the input to the doxygen
	# executable
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.cmake
			${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)

	# now add the custom target. This will create a build target called 'DOCUMENTATION'
	# in your project
	ADD_CUSTOM_TARGET(docs
			COMMAND pwd
			COMMAND echo ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
			COMMAND ${CMAKE_COMMAND} -E echo_append "Building API Documentation..."
			COMMAND ${CMAKE_COMMAND} -E make_directory ${DOXYGEN_OUTPUT_DIR}
			COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
			WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
			COMMAND ${CMAKE_COMMAND} -E echo "Done."
			WORKING_DIRECTORY ${DOXYGEN_OUTPUT_DIR})

endif (DOXYGEN_FOUND)

find_package(CxxTest)
if(CXXTEST_FOUND)
	include_directories(${CXXTEST_INCLUDE_DIR})
	enable_testing()
	set(Tests
		${CMAKE_CURRENT_SOURCE_DIR}/test/AbstractConfigTest.h)
	CXXTEST_ADD_TEST(config-tests config.cc ${Tests})
	if(FILE)
		include_directories(test/file_config)
	elseif(ZOO)
		include_directories(test/zoo_config)
	else()
		message(FATAL_ERROR "Only one of FILE or ZOO must be set.")
	endif(FILE)
	target_link_libraries(config-tests ns-skeleton)
	add_custom_command(TARGET config-tests
                        POST_BUILD
                        COMMAND ${CMAKE_CURRENT_BINARY_DIR}/config-tests -v)
endif(CXXTEST_FOUND)

install(TARGETS ns-skeleton
        LIBRARY DESTINATION lib/nanoservices
        COMPONENT lib)

install(FILES ns-skeleton.conf
        DESTINATION /etc/ld.so.conf.d/
        COMPONENT lib)

install(FILES FindNanoservices.cmake
        DESTINATION /usr/share/cmake-3.10/Modules/
        COMPONENT dev)

install(DIRECTORY include/
        DESTINATION /usr/include/nanoservices
        COMPONENT dev)

install(DIRECTORY ${nanoservices_SOURCE_DIR}/third-party/msgpack-c/include/
        DESTINATION /usr/include
        COMPONENT dev)

set(CPACK_DEBIAN_LIB_PACKAGE_CONTROL_EXTRA postinst)
