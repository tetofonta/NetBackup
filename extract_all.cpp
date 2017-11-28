//
// Created by stefano on 04/11/17.
//
#include "AES/aes.h"
#include <stdio.h>
#include <inttypes.h>
#include <memory.h>
#include <cstdlib>
#include "FileSystem/FS.h"
#include "Headers.h"

char key[32];
char nome[500];

uint8_t * data;

int extract(int packetSize, char * key_aes, char * file){

    bzero(key, 32);
    for(int i = 0; i < 32; i++) key[i] =key_aes[i];

    FILE *in = fopen(file, "r");

    int zip = 0;
    if(file[strlen(file) - 1] == '2') zip = 1;

    file_h header;
    int current = 0;
    while(!feof(in)){
        size_t reade = fread(&header, 1, sizeof(header), in);
        if(reade < sizeof(header)) continue;
        size_t len = strlen(header.name);
        if(len < 5) return -1;
        header.name[len-4] = '\0';

        FILE *out;
        if(current++ == 0){
            out = fopen(header.name, "w");
        } else {
            sprintf(nome, "%s_%d", header.name, current);
            out = fopen(nome, "w");
        }

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

            fread(data, 1, (size_t) packetSize, in);
            if(header.isEncoded){
                for(int q = 0; q < packetSize/16; q++)
                    AES_ECB_decrypt(data+(q*16), (uint8_t *) key, data+(q*16), 16);
            }

            size_t tow = (rimanenteDaScrivere > (uint64_t) packetSize) ? (size_t) packetSize : rimanenteDaScrivere;
            size_t wrt = fwrite(data, 1, tow, out);
            fflush(out);
            rimanenteDaScrivere -= wrt;
        }
        printf("\r"); fflush(stdout);
        for(int q = 0; q < 30; q++){
            printf("#");
            fflush(stdout);
        }

        printf("[100%%]");
        printf("\e[?25h \n");
        fflush(stdout);

        fclose(out);
    }
    fclose(in);

    return 0;
}

int main(int argc, char ** argv){
    if(argc < 4)
        printf("Usage ./extract_all <directory> <transfer_lenght> <key>\n");

    int packetSize = atoi(argv[2]);

    int len;
    char ** out = getFiles(argv[1], &len, 1);

    for(int i = 0; i < len; i++){
        if(strcmp(out[i] + strlen(out[i])-3, "bak") == 0) {
            printf("%s\n", out[i]);
            extract(packetSize, argv[3], out[i]);
        }
    }
}
