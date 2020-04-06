cmake_minimum_required(VERSION 3.0)
project(ns_configurator)

set(Sources 
		main.cpp
		${ns_skeleton_SOURCE_DIR}/src/NsException.cpp
		${ns_skeleton_SOURCE_DIR}/src/NsSkelJson.cpp
		${ns_skeleton_SOURCE_DIR}/src/NsUtils.cpp
		${ns_skeleton_SOURCE_DIR}/src/ConfigValidator.cpp
		${ns_skeleton_SOURCE_DIR}/src/CVInvalidType.cpp
		${ns_skeleton_SOURCE_DIR}/src/CVNoNodeData.cpp
		${ns_skeleton_SOURCE_DIR}/src/CVTooManyChildren.cpp
		${ns_skeleton_SOURCE_DIR}/src/CVTooLongData.cpp
		${ns_skeleton_SOURCE_DIR}/src/NsCmdLineParameters.cpp)

if(FILE)
	set(Sources ${Sources}
		${ns_skeleton_SOURCE_DIR}/src/file/FileConfig.cpp)
elseif(ZOO)
	set(Sources ${Sources}
		${ns_skeleton_SOURCE_DIR}/src/zoo/ZooConfig.cpp)
else()
	message(FATAL_ERROR "You cmake it wrong!!")
endif(FILE)

include_directories(${ns_skeleton_SOURCE_DIR}/include)
# Header-only MSGPACK realisation
include_directories(${nanoservices_SOURCE_DIR}/third-party/msgpack-c/include)

if(FILE)
	include_directories(${ns_skeleton_SOURCE_DIR}/include/file)
elseif(ZOO)
	include_directories(${ns_skeleton_SOURCE_DIR}/include/zoo)
	include_directories(/usr/include/zookeeper)
	set(CONFIG_LIBS zookeeper_mt)
else()
	message(FATAL_ERROR "You cmake it wrong!!")
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
	message(FATAL_ERROR "You cmake it wrong!!")
endif(FILE)

add_executable(ns-configurator ${Sources} ${Headers})

find_package(Threads)
target_link_libraries(ns-configurator ${CMAKE_THREAD_LIBS_INIT} ${CONFIG_LIBS})

install(TARGETS ns-configurator
        RUNTIME DESTINATION bin
        COMPONENT misc)

