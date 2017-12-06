/**
 * \file IO.c
 * \version 1.0
 * \author Stefano
 * \date 28-10-2017
 * \brief IO RSA
 */
#include <malloc.h>
#include "internal.h"

char * line;
mpz_t rsaPrams[7];
int errorHappened = 0;
char errorMessage[100];
const char rsap[7] = {'p', 'q', 'n', 'e', 'd', 'n', 'e'};

/**
 * \brief Carica le informazioni relative a una coppia di chiavi RSA da un file .rsacfg
 * In caso di errore viene settata la variabile errorHappened e il relativo messaggio
 * \see RSA_check_errors()
 * \see RSA_error_get_last()
 * @param file[in] File di input
 */
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

/**
 * \brief Carica le informazioni relative a una chiave pubblica RSA da un file .public
 * @param file[in] File di input
 * @param supplementare[in] se 0 verranno caricati i parametri della coppia di chiavi, altrimenti verranno caricati nei valori paralleli.
 */
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