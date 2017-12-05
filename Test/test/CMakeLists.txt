include(CTest)
enable_testing()

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(FS_t FileSystem.cpp)
target_link_libraries(FS_t gtest gtest_main)
target_link_libraries(FS_t _FS)
add_test(FileSystem_test FS_t)

add_executable(AES_t Aes.cpp)
target_link_libraries(AES_t gtest gtest_main)
target_link_libraries(AES_t _AES)
add_test(AES_test AES_t)

add_executable(B32_t Base32.cpp)
target_link_libraries(B32_t gtest gtest_main)
target_link_libraries(B32_t _B32)
add_test(Base32_test B32_t)

add_executable(CFG_t Config.cpp)
target_link_libraries(CFG_t gtest gtest_main)
target_link_libraries(CFG_t _CFG)
add_test(Configuration_test CFG_t)

add_executable(MD5_t Md5.cpp)
target_link_libraries(MD5_t gtest gtest_main)
target_link_libraries(MD5_t _MD5)
add_test(MD5_test MD5_t)

#add_executable(NET_t Networking.cpp)
#target_link_libraries(NET_t gtest gtest_main)
#target_link_libraries(NET_t _NET)
#add_test(NAME FileSystem_test COMMAND FS_t)