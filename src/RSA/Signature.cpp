/**
 * \file Signature.c
 * \version 1.0
 * \author Stefano
 * \date 28-10-2017
 * \brief Firma digitale RSA
 */
#include "RSA.h"
#include "internal.h"

/**
 * \brief Firma digitalmente una stringa data
 * @param ret[out] Numero contenente il messaggio firmato
 * @param data[in] Messaggio da firmare
 */
void sign(mpz_ptr ret, const mpz_t data){
    mpz_powm_sec(ret, data, rsaPrams[4], rsaPrams[2]);
}

/**
 * \brief rimuove la firma digitale da una stringa data
 * @param ret[out] Numero contenente il messaggio originale
 * @param data[in] Messaggio firmato
 * @param supplementare[in] se 0 verranno utilizzati i parametri pubblici della chiave RSA primaria, altrimenti verranno utilizzati i paramentri paralleli.
 */
void unsign(mpz_ptr ret, const mpz_t data, int supplementare){
    if(supplementare) supplementare = 3;
    mpz_powm_sec(ret, data, rsaPrams[3+supplementare], rsaPrams[2+supplementare]);
}

