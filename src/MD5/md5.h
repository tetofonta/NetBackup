//
// Created by stefano on 21/11/17.
//

#ifndef NB_MD5_H
#define NB_MD5_H

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
void md5(uint8_t *initial_msg, size_t initial_len, char *);
#ifdef __cplusplus
}
#endif
#endif //NB_MD5_H
