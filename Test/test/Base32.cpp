//
// Created by stefano on 03/12/17.
//

#include "gtest/gtest.h"
#include "BASE_encoding/base32.h"

void testEncoding(const char * plain, const char * expected){
    char encoded[BASE32_LEN(strlen(plain)) + 1];
    encoded[BASE32_LEN(strlen(plain))] = '\0';
    base32_encode((const unsigned char *) plain, strlen(plain), (unsigned char *) encoded);
    ASSERT_EQ(strlen(encoded), BASE32_LEN(strlen(plain)));
    ASSERT_STREQ(encoded, expected);
}

void testDecoding(const char * plain, const char * expected){
    char encoded[UNBASE32_LEN(strlen(plain)) + 1];
    encoded[UNBASE32_LEN(strlen(plain))] = '\0';
    base32_decode((const unsigned char *) plain, (unsigned char *) encoded);
    ASSERT_STREQ(encoded, expected);
}


TEST(base32, encode){
    testEncoding("StefanoFontana4DITestSession123456789\03", "KN2GKZTBNZXUM33OORQW4YJUIREVIZLTORJWK43TNFXW4MJSGM2DKNRXHA4QG===");
    testEncoding("", "");
}

TEST(base32, decode){
    testDecoding("KN2GKZTBNZXUM33OORQW4YJUIREVIZLTORJWK43TNFXW4MJSGM2DKNRXHA4QG===", "StefanoFontana4DITestSession123456789\03");
    testDecoding("", "");
}