//
// Created by stefano on 05/12/17.
//

#include <gtest/gtest.h>
#include <gmp.h>
#include "RSA/RSA.h"

static char rndChar() {
    char possibili[] = "abcdef01234567890";
    return possibili[rand() % 16];
}


TEST(RSA, encrypt){
    char messaggio[] = "stefanoFontana";
    char messaggio_esad[30];
    for(int i = 0; i < 14; i++)sprintf(messaggio_esad + i *2, "%2.2x", messaggio[i] & 0xFF);

    mpz_t e;
    mpz_t n;
    mpz_init_set_str(e, "010001", 16);
    mpz_init_set_str(n, "c5441085efc541103f0d0d465bc332b79bce2be68b06671f489d745154f39fa712d732d0a7613f27cd3111cbb78bbce526086846b540eec29b01409c7af946e978a404c798bc80de472e4203fb0b51b48de7eb2d3ff8ebf545e53eb54d1af94d7cb9f73d64a58e8069634bc8356500ea6523453936094a9e41da8bec7bf67fdd", 16);

    mpz_t check;
    mpz_init_set_str(check, messaggio_esad, 16);

    mpz_powm_sec(check, check, e, n);

    char *returned = mpz_get_str(NULL, 16, check);
    mpz_clear(check);
    mpz_clear(e);
    mpz_clear(n);

    ASSERT_STREQ(returned, "4f2654003cffda799fbedc97678b01c6b760cb0a22e17805631e226e3b77bc8037edd87a889406f0f6750e42ec21a3155aee5c053caeacf38b3db14fc0a65701c8ca780e02893e0e8ea7184973fbedded3708d4736ceb8e54b2f394e9736f9a068ef00a267a895ef07ffad1c396fad1dcea21ea8db48789fd4d9858cdd896a98");
}

TEST(RSA, decrypt){
    char messaggio[] = "4f2654003cffda799fbedc97678b01c6b760cb0a22e17805631e226e3b77bc8037edd87a889406f0f6750e42ec21a3155aee5c053caeacf38b3db14fc0a65701c8ca780e02893e0e8ea7184973fbedded3708d4736ceb8e54b2f394e9736f9a068ef00a267a895ef07ffad1c396fad1dcea21ea8db48789fd4d9858cdd896a98";;

    mpz_t n;
    mpz_t d;
    mpz_init_set_str(n, "c5441085efc541103f0d0d465bc332b79bce2be68b06671f489d745154f39fa712d732d0a7613f27cd3111cbb78bbce526086846b540eec29b01409c7af946e978a404c798bc80de472e4203fb0b51b48de7eb2d3ff8ebf545e53eb54d1af94d7cb9f73d64a58e8069634bc8356500ea6523453936094a9e41da8bec7bf67fdd", 16);
    mpz_init_set_str(d, "be09f46113bc5411f2ac003de7e904d31da66164984a72ce10869fa83acef8df5ba735ad0d9301e4b628af9c3216ccc375006474b9e8f41b4678ce65f192745c8633d1b5981ca06ae16218faf1837a4ab882ffa25c26dacfc2da2363bf3cb5a320c41e1d49518a314ad02635a5dff6b90caed76b9f9957161a2192d96d847c1", 16);

    mpz_t check;
    mpz_init_set_str(check, messaggio, 16);

    mpz_powm_sec(check, check, d, n);

    char *returned = mpz_get_str(NULL, 16, check);
    mpz_clear(check);
    mpz_clear(n);
    mpz_clear(d);

    ASSERT_STREQ(returned, "73746566616e6f466f6e74616e61");
}

TEST(RSA, encDec){
    char messaggio_esad[99];
    for(int i = 0; i < 98; i++) messaggio_esad[i] = rndChar();
    messaggio_esad[98] = '\0';

    mpz_t e;
    mpz_t n;
    mpz_t d;
    mpz_init_set_str(e, "010001", 16);
    mpz_init_set_str(n, "c5441085efc541103f0d0d465bc332b79bce2be68b06671f489d745154f39fa712d732d0a7613f27cd3111cbb78bbce526086846b540eec29b01409c7af946e978a404c798bc80de472e4203fb0b51b48de7eb2d3ff8ebf545e53eb54d1af94d7cb9f73d64a58e8069634bc8356500ea6523453936094a9e41da8bec7bf67fdd", 16);
    mpz_init_set_str(d, "be09f46113bc5411f2ac003de7e904d31da66164984a72ce10869fa83acef8df5ba735ad0d9301e4b628af9c3216ccc375006474b9e8f41b4678ce65f192745c8633d1b5981ca06ae16218faf1837a4ab882ffa25c26dacfc2da2363bf3cb5a320c41e1d49518a314ad02635a5dff6b90caed76b9f9957161a2192d96d847c1", 16);

    mpz_t check;
    mpz_init_set_str(check, messaggio_esad, 16);

    mpz_powm_sec(check, check, e, n);
    mpz_powm_sec(check, check, d, n);

    char *returned = mpz_get_str(NULL, 16, check);
    mpz_clear(check);
    mpz_clear(e);
    mpz_clear(n);
    mpz_clear(d);

    ASSERT_STREQ(returned, messaggio_esad);
}