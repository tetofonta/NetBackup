//
// Created by stefano on 02/12/17.
//
#include "gtest/gtest.h"
#include "FileSystem/FS.h"

TEST(FileSystem, isFile){
    EXPECT_EQ(0, is_regular_file("/home/stefano"));
    EXPECT_EQ(0, is_regular_file("."));
    EXPECT_EQ(0, is_regular_file(".."));
    EXPECT_EQ(0, is_regular_file("./.."));
    EXPECT_EQ(1, is_regular_file("./FS_t"));
    EXPECT_EQ(0, is_regular_file("/unexistant/directory/"));
}

TEST(FileSystem, dirOut){
    char ** out;
    int len;
    out = getFiles("/home/stefano/Scrivania/", &len, 2);
    //for(int i = 0; i < len-2; i++) std::cout << out[i];
}