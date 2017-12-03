//
// Created by stefano on 10/11/17.
//

#ifndef NB_BASE32_H
#define NB_BASE32_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BASE32_LEN(len)  (((len)/5)*8 + ((len) % 5 ? 8 : 0))
#define UNBASE32_LEN(len)  (((len)/8)*5)

void base32_encode(const unsigned char *plain, size_t len, unsigned char *coded);
size_t base32_decode(const unsigned char *coded, unsigned char *plain);

#ifdef __cplusplus
}
#endif

#endif //NB_BASE32_H
