/**
 * \file utilities.c
 * \version 1.0
 * \author Stefano
 * \date 28-10-2017
 * \brief Utilità RSA
 */
#include <malloc.h>
#include "internal.h"


/**
 * \brief Controllo di avvenuto errore per il protocollo RSA
 * @return 0 se non è avvenuto alcun errore, 1 altrimenti.
 */
int RSA_check_errors(){
    return errorHappened;
}

/**
 * \brief Ottiene la stringa relativa all'errore
 * @return Un puntatore alla stringa con il messaggio di errore
 */
char * RSA_error_get_last(){
    return errorMessage;
}

/**
 * \brief Converte una strnga esadecimale in numero
 * @param ret[out] Numero di output
 * @param data[in] Stringa
 * @param size[in] lunghezza della stringa
 */
void msgToNumber(mpz_ptr ret, const char * data, const int size){
    char * hexPart = (char *) malloc((2 * size + 1)*sizeof(char));
    for(int i = 0; i < size; i++) sprintf(hexPart+(2*i), "%2.2x", data[i] & 0xFF);
    mpz_init_set_str(ret, hexPart, 16);
    free(hexPart);
}