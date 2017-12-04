cmake_minimum_required(VERSION 2.8.4)
project(NetBackup)

add_definitions(-std=gnu++11)

enable_language(C)
enable_language(CXX)

if (CMAKE_CXX_COMPILER_ID MATCHES GNU)
    set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
endif ()

include_directories(src)
add_subdirectory(Test)
add_subdirectory(src)