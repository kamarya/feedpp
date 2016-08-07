cmake_minimum_required (VERSION 3.2)
project (feed_plus_plus)


set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}")
message (STATUS ${PROJECT_SOURCE_DIR})


find_package(CURL)
include_directories(${CURL_INCLUDE_DIRS})

find_package(LibXml2 REQUIRED)
include_directories(${LIBXML2_INCLUDE_DIR})

find_package(Threads)

include_directories(inc)

file(GLOB_RECURSE SRC_FILES ${PROJECT_SOURCE_DIR}/src/*.cpp)

add_executable(utest ${SRC_FILES})
target_link_libraries(utest ${CURL_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${LIBXML2_LIBRARIES})

enable_testing ()
add_test (NAME UnitTest COMMAND ./utest --verbose)
