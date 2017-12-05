//
// Created by stefano on 04/12/17.
//

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "config/config.h"

int makeTestCfgFile() {
    FILE *fp = fopen("test.properties", "w");
    if (fp == NULL) return 1;
    int len = fprintf(fp, "#Commento\n"
            "should_Be_True = 1\n"
            "should_contain_spaces = string~with~spaces\n\n"
            "That_is_a_list = :\n"
            "Element\n"
            "Element~with~spaces\n"
            "::\n"
            "_STOP_PARSING_\n"
            "unaccessible_properties = ciao\n");
    fflush(fp);
    fclose(fp);
    return len;
}

TEST(config, createFile) {
    int ret = makeTestCfgFile();
    ASSERT_NE(1, ret) << "Cannot create test file";
    ASSERT_LT(1, ret) << "Cannot write on file";
}

char **descriptor;
char tobe[][1000] = {"should_Be_True=1",
                     "should_contain_spaces=string with spaces",
                     "That_is_a_list=:",
                     "Element",
                     "Element with spaces",
                     "::"};

void openFile_test(){
    int linee = 0;
    makeTestCfgFile();
    descriptor = openConfigFile(&linee, "test.properties", '#');
    ASSERT_EQ(6, linee);
    for (int i = 0; i < linee; i++)
        EXPECT_STREQ(descriptor[i], tobe[i]);
}

TEST(config, openFile) {
    makeTestCfgFile();
    openFile_test();
}

TEST(config, getPropoerty){

    makeTestCfgFile();
    openFile_test();
    int isList, idx;
    char * ret = getProperty(descriptor, 6, "should_Be_True", 100, &isList, &idx);
    EXPECT_STREQ("1", ret);
    EXPECT_EQ(0, isList);

    ret = getProperty(descriptor, 6, "should_contain_spaces", 100, &isList, &idx);
    EXPECT_STREQ("string with spaces", ret);
    EXPECT_EQ(0, isList);

    ret = getProperty(descriptor, 6, "That_is_a_list", 100, &isList, &idx);
    EXPECT_EQ(1, isList);
    EXPECT_EQ(2, idx);
    EXPECT_EQ(3, (int)(long) ret);

    ret = getProperty(descriptor, 6, "unreachable_property", 100, &isList, &idx);
    char * nullpt = NULL;
    EXPECT_STREQ(nullpt, ret);
}