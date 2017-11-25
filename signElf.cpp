//
// Created by stefano on 23/11/17.
//
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "MD5/md5.h"

#ifdef GMP
#include "RSA/RSA.h"
#include "RSA/internal.h"

char * buffer;
char md[16];
char foo[33];

int main(int argc, char ** argv){
    if(argc < 2) return -6;

    FILE * fp = fopen(argv[1], "a+");
    if(fp == NULL)
        return -3;

    fseek(fp, 0, SEEK_END);
    size_t len = ftell(fp);
    rewind(fp);
    buffer = (char *) malloc(len);
    fread(buffer, 1, len, fp);
    md5((uint8_t *) buffer, len, md);

    for(int i = 0; i < 16; i++) sprintf(foo + (i*2), "%2.2x", md[i] & 0xFF);
    foo[32] = '\0';

    loadValues("CA.rsacfg");

    mpz_t tosign;
    mpz_init_set_str(tosign, foo, 16);
    sign(tosign, tosign);
    char * signed_md5 = mpz_get_str(NULL, 16, tosign);

    fwrite(signed_md5, 1, strlen(signed_md5), fp);
    int foo = strlen(signed_md5);
    fwrite(&foo, sizeof(int), 1, fp);

    free(buffer);
    fclose(fp);
}

#else
int main(void){
    printf("NO GMP LIB\n");
}
#endif
