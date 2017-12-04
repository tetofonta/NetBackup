include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(FS_t FileSystem.cpp)
target_link_libraries(FS_t gtest gtest_main)
target_link_libraries(FS_t _FS)

add_executable(AES_t Aes.cpp)
target_link_libraries(AES_t gtest gtest_main)
target_link_libraries(AES_t _AES)

add_executable(B32_t Base32.cpp)
target_link_libraries(B32_t gtest gtest_main)
target_link_libraries(B32_t _B32)

add_executable(CFG_t Config.cpp)
target_link_libraries(CFG_t gtest gtest_main)
target_link_libraries(CFG_t _CFG)

add_executable(MD5_t Md5.cpp)
target_link_libraries(MD5_t gtest gtest_main)
target_link_libraries(MD5_t _MD5)