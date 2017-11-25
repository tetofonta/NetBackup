//
// Created by stefano on 28/10/17.
//
#include "RSA.h"
#include "internal.h"
#include <malloc.h>

char * line;
mpz_t rsaPrams[7];
int errorHappened = 0;
char errorMessage[100];
const char rsap[7] = {'p', 'q', 'n', 'e', 'd', 'n', 'e'};

void loadValues(const char * file){
    FILE * fp = fopen(file, "r");

    if(fp ==  NULL){
        errorHappened = 1;
        sprintf(errorMessage, "File %s not found", file);
        return;
    }

    size_t lenght;

    for(int i = 0; i < 5; i++){
        if (getline(&line, &lenght, fp) > 0){
            mpz_init_set_str(rsaPrams[i], line, 16);
        } else{
            errorHappened = 1;
            sprintf(errorMessage, "Cannot load %c RSA parameter: loading stopped", rsap[i]);
        };
    }

    free(line);
}

void loadpkey(const char * file, int supplementare){

    int offset = 0;
    if(supplementare) offset = 3;

    FILE * fp = fopen(file, "r");

    if(fp ==  NULL){
        errorHappened = 1;
        sprintf(errorMessage, "File %s not found", file);
        return;
    }

    size_t lenght;

    for(int i = 2+offset; i < 4+offset; i++){
        if (getline(&line, &lenght, fp) > 0){
            mpz_init_set_str(rsaPrams[i], line, 16);
        } else{
            errorHappened = 1;
            sprintf(errorMessage, "Cannot load %c RSA parameter: loading stopped", rsap[i]);
        };
    }

    free(line);
}