set(Sources 
		main.cpp
		../ns-skeleton/src/NsException.cpp
		../ns-skeleton/src/NsSkelJson.cpp
		../ns-skeleton/src/ConfigValidator.cpp
		../ns-skeleton/src/CVInvalidType.cpp
		../ns-skeleton/src/CVNoNodeData.cpp
		../ns-skeleton/src/CVTooManyChildren.cpp
		../ns-skeleton/src/CVTooLongData.cpp
		../ns-skeleton/src/NsCmdLineParameters.cpp)

if(FILE)
	set(Sources ${Sources}
		../ns-skeleton/src/file/FileConfig.cpp)
elseif(ZOO)
	set(Sources ${Sources}
		../ns-skeleton/src/zoo/ZooConfig.cpp)
else()
	message(FATAL_ERROR "You cmake it wrong!!")
endif(FILE)

include_directories(../ns-skeleton/include)

if(FILE)
	include_directories(../ns-skeleton/include/file)
elseif(ZOO)
	include_directories(../ns-skeleton/include/zoo)
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
        RUNTIME DESTINATION bin/nanoservices
        COMPONENT misc)
