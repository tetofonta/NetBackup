//extern "C"
// Created by stefano on 28/10/17.
//
#include "RSA.h"
#include "internal.h"

void sign(mpz_ptr ret, const mpz_t data){
    mpz_powm_sec(ret, data, rsaPrams[4], rsaPrams[2]);
}

void unsign(mpz_ptr ret, const mpz_t data, int supplementare){
    if(supplementare) supplementare = 3;
    mpz_powm_sec(ret, data, rsaPrams[3+supplementare], rsaPrams[2+supplementare]);
}

