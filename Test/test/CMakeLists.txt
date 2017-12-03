include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(FS_t FileSystem.cpp)
target_link_libraries(FS_t gtest gtest_main)
target_link_libraries(FS_t _FS)

include(CTest)
enable_testing()
add_test(FileSystem FileSystem.FileSystemTests)