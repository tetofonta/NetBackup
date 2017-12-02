//
// Created by stefano on 28/10/17.
//

#ifndef NB_RSA_H
#define NB_RSA_H

#ifdef GMP
#include <gmp.h>

void loadValues(const char * file);
void sign(mpz_ptr ret, const mpz_t data);
void unsign(mpz_ptr ret, const mpz_t data, int);
int RSA_check_errors();
char * RSA_error_get_last();
void msgToNumber(mpz_t ret, const char * data, const int size);
void loadpkey(const char * file, int);
#endif

#endif //NB_RSA_H
