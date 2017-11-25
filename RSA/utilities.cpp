//
// Created by stefano on 28/10/17.
//
#include <malloc.h>
#include "internal.h"
#include "RSA.h"

int RSA_check_errors(){
    return errorHappened;
}

char * RSA_error_get_last(){
    return errorMessage;
}

void msgToNumber(mpz_ptr ret, const char * data, const int size){
    char * hexPart = (char *) malloc((2 * size + 1)*sizeof(char));
    for(int i = 0; i < size; i++) sprintf(hexPart+(2*i), "%2.2x", data[i] & 0xFF);
    mpz_init_set_str(ret, hexPart, 16);
    free(hexPart);
}