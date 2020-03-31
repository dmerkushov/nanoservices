project(test_service)

set(Source main.cpp)

include_directories(/usr/include/nanoservices)

add_executable(testService ${Source})
target_link_libraries(testService ns-skeleton)
