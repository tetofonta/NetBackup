/**
 * \file genPassword.c
 * \version 1.0
 * \author Stefano
 * \date 23-11-2017
 * \brief Utilità di generazione hash e codifica password
 */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <malloc.h>
#include "MD5/md5.h"
#include "BASE_encoding/base32.h"

int main(int argc, char **argv) {

    if (argc < 2) {
        printf("usage: %s 'string'\n", argv[0]);
        return 1;
    }

    char *msg = argv[1];
    size_t len = strlen(msg);

    char a[10000];
    bzero(a, 10000);
    md5((uint8_t *) msg, len, a);

    printf("HASH: ");
    for(int i = 0; i < 16; i++) printf("%2.2x", a[i] & 0xFF);
    printf("\n");

    char * coded = (char *) malloc(2000);
    base32_encode((const unsigned char *) msg, strlen(msg), (unsigned char *) coded);
    printf("ENCODED KEY: %s\n", coded);

    free(coded);
    return 0;
}