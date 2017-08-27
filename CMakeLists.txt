cmake_minimum_required (VERSION 3.7)
project (FeedPP)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra")

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.9)
        set(BOOST_COMPONENTS system coroutine context thread)
        set(BOOST_COMPONENTS ${BOOST_COMPONENTS} regex)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_BOOST_REGEX")
        find_package(Boost 1.54.0 COMPONENTS ${BOOST_COMPONENTS} REQUIRED)
        include_directories(SYSTEM ${Boost_INCLUDE_DIR})
    endif()
endif()

find_package(LibXml2 REQUIRED)
include_directories(${LIBXML2_INCLUDE_DIR})

find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIR})

include_directories (inc)
set (SRC_FILES src/atom_parser.cpp
               src/exception.cpp
               src/date.cpp
               src/parser.cpp
               src/parser_factory.cpp
               src/rss_09x_parser.cpp
               src/rss_10_parser.cpp
               src/rss_parser.cpp
               src/unitest.cpp
               src/utils.cpp)

add_library (feedpp ${SRC_FILES})

add_executable(unitest src/unitest.cpp)
target_link_libraries(unitest feedpp)
target_link_libraries(unitest ${Boost_LIBRARIES})
target_link_libraries(unitest ${LIBXML2_LIBRARIES})
target_link_libraries(unitest ${CURL_LIBRARIES})
