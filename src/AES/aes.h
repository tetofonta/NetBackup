//
// Created by stefano on 31/10/17.
//

#ifndef NB_AES_H
#define NB_AES_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ECB 1
#define AES256 1

void AES_ECB_encrypt(const uint8_t *input, const uint8_t *key, uint8_t *output, const uint32_t length);

void AES_ECB_decrypt(const uint8_t *input, const uint8_t *key, uint8_t *output, const uint32_t length);

#ifdef __cplusplus
}
#endif

#endif //NB_AES_H
