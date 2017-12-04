//
// Created by stefano on 04/12/17.
//

#include "gtest/gtest.h"
#include "MD5/md5.h"

void getDigest(const char * ret, char * digest){
    for(int i = 0; i < 16; i++) sprintf(digest + 2*i, "%2.2x", ret[i] & 0xFF);
}

void doMd5(const char * str, char * out){
    char ret[16];
    uint8_t * foo = (uint8_t *) str;
    size_t len = (str == NULL) ? 0 : strlen(str);
    md5(foo, len, ret);
    getDigest(ret, out);
}

TEST(md5, hashing){
    char digest[33];
    doMd5("stefano", digest);
    ASSERT_STREQ("317a58affea472972b63bffdd3392ae0", digest);
    doMd5("", digest);
    ASSERT_STREQ("d41d8cd98f00b204e9800998ecf8427e", digest);
    doMd5(NULL, digest);
    ASSERT_STREQ("d41d8cd98f00b204e9800998ecf8427e", digest);
}