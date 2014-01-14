

#ifndef DCRYPT_H
#define DCRYPT_H

#include "init.h"
#include "sha256.h"
 
//the dcrypt hashing algorithm, for multiple strings
uint32_t dcrypt(char **str_nums, uint32_t n_str, char *out_hash, uint8_t *hash_digest);

//the dcrypt hashing algorithm for a single string
uint32_t dcrypt(char *str_nums, char *out_hash, uint8_t *hash_digest);

#endif
