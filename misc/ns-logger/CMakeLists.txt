cmake_minimum_required(VERSION 3.0)
project(ns-logger)

set(CMAKE_CXX_STANDART 11)

set(Sources main.cpp Logger.cpp)
set(Headers Logger.h)

add_executable(ns-logger ${Sources} ${Headers})

find_package(Nanoservices)

include_directories(${LIBNSSKELETON_INCLUDE_DIRS})
target_link_libraries(ns-logger ${LIBNSSKELETON_LIBRARIES})

SET(CPACK_GENERATOR "DEB")
SET(CPACK_PACKAGE_VENDOR "Nanoservice consorcium")
SET(CPACK_PACKAGE_CONTACT "petelin.alexey@gmail.com")
SET(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
SET(CPACK_PACKAGE_VERSION 0.1)
SET(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")
SET(CPACK_DEBIAN_PACKAGE_DEPENDS "nanoservices, nanoservices-misc")
SET(CPACK_DEBIAN_PACKAGE_DESCRIPTION "Nanoservice for logging nanoservices")
SET(CPACK_DEBIAN_FILE_NAME  DEB-DEFAULT)

install(TARGETS ns-logger
        RUNTIME DESTINATION bin
        COMPONENT service)

install(FILES ns-logger.json
        DESTINATION /tmp/ns-logger/
        COMPONENT service)

install(PROGRAMS ns-logger
        DESTINATION /etc/init.d/
        COMPONENT service)

SET(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${CMAKE_CURRENT_SOURCE_DIR}/postinst")
INCLUDE(CPack)
