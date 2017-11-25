//
// Created by stefano on 04/11/17.
//
#include "AES/aes.h"
#include <stdio.h>
#include <inttypes.h>
#include <memory.h>
#include <cstdlib>
#ifdef BZIP
#include <bzlib.h>
#endif
#include "Headers.h"


#ifndef BZIP
#define BZFILE FILE
#endif


char key[32];

uint8_t * data;

int main(int argc, char ** argv){
    if(argc < 3){
        printf("USAGE %s input_file transfer_len encryption_key [output_file]\n", argv[0]);
        return -6;
    };
    int packetSize = atoi(argv[2]);

    bzero(key, 32);
    for(int i = 0; i < 32; i++) key[i] = argv[3][i];

    FILE *in = fopen(argv[1], "r");

    int zip = 0;
    if(argv[1][strlen(argv[1]) - 1] == '2') zip = 1;

    file_h header;

    BZFILE * zip_p;
    int err;
    if(zip){
#ifdef BZIP
        BZ2_bzReadOpen(&err, in, 0, 0, NULL, 0);
#endif
    }

    if(zip){
#ifdef BZIP
        BZ2_bzRead(&err, zip_p, &header, sizeof(header));
#endif
    }
    else fread(&header, sizeof(header), 1, in);
    size_t len = strlen(header.name);
    if(len < 5) return -1;
    header.name[len-4] = '\0';

    FILE *out;
    if(argc == 5) out = fopen(argv[4], "w");
    else out = fopen(header.name, "w");

    data = (uint8_t *) malloc(packetSize * sizeof(uint8_t));

    uint64_t rimanenteDaScrivere = header.dimension;

    printf("\e[?25l");
    fflush(stdout);
    for(int i = 0; i < header.transfer_dimension; i += packetSize){

        printf("\r"); fflush(stdout);
        for(int q = 0; q < (i*30/header.transfer_dimension); q++){
            printf("#");
            fflush(stdout);
        }

        printf("[%d%%]", (i*100)/header.transfer_dimension);
        fflush(stdout);

        size_t readed;

        if(zip){
#ifdef BZIP
            readed = (size_t)BZ2_bzRead(&err, zip_p, data, packetSize);
#endif
        }
        else readed = fread(data, 1, (size_t) packetSize, in);

        if(header.isEncoded){
            for(int q = 0; q < packetSize/16; q++)
                AES_ECB_decrypt(data+(q*16), (uint8_t *) key, data+(q*16), 16);
        }

        size_t tow = (rimanenteDaScrivere > (uint64_t) packetSize) ? (size_t) packetSize : rimanenteDaScrivere;
        size_t wrt = fwrite(data, 1, tow, out);
        fflush(out);
        rimanenteDaScrivere -= wrt;
    }
    printf("\e[?25h \n");
    fflush(stdout);

    fclose(out);
#ifdef BZIP
    BZ2_bzReadClose(&err, zip_p);
#endif
    fclose(in);
    return 0;
}