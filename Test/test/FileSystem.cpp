//
// Created by stefano on 02/12/17.
//
#include "gtest/gtest.h"
#include "FileSystem/FS.h"

TEST(FileSystem, is_regular_file){
    ASSERT_EQ(0, is_regular_file(".")) << ". is seen as a regular file";
    ASSERT_EQ(0, is_regular_file("..")) << ".. is seen as a regular file";
    ASSERT_EQ(0, is_regular_file("./..")) << "./.. is seen as a regular file";
    ASSERT_EQ(1, is_regular_file("./FS_t")) << "myself is not seen as a regular file";
    ASSERT_EQ(0, is_regular_file("./unexistant.file"));
    ASSERT_EQ(0, is_regular_file("/unexistant/directory/")) << "Seriusly have you got a directoty named /unexistant/directory/ ?????";
}

TEST(FileSystem, getFiles){
    char ** out;
    int len;
    ASSERT_NO_FATAL_FAILURE((out = getFiles("./", &len, 2)));
    EXPECT_LT(0, len);
}

