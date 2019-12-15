cmake_minimum_required(VERSION 3.0)
project(ns-skeleton)

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
		src/NsSkelUtils.cpp)

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
		include/NsSkelUtils.h)

include_directories(include)

add_library(ns-skeleton SHARED ${Sources} ${Headers})

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