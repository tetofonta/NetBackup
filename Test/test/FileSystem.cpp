//
// Created by stefano on 02/12/17.
//
#include "gtest/gtest.h"
#include "FileSystem/FS.h"

TEST(isFile, test_isFile){
    EXPECT_EQ(0, is_regular_file("/home/stefano/"));
}