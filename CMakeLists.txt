cmake_minimum_required(VERSION 2.8.4)
project(NetBackup)

add_definitions(-std=gnu++11)

include_directories(src)
add_subdirectory(Test)
add_subdirectory(src)