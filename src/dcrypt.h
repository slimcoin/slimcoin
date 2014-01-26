
#ifndef DCRYPT_H
#define DCRYPT_H

#include "sha256.h"

#define DCRYPT_DIGEST_LENGTH SHA256_DIGEST_LENGTH 

//the dcrypt hashing algorithm, for multiple strings
uint256 dcrypt(char **str_nums, uint32_t n_str, char *out_hash, uint8_t *hash_digest = 0);

//the dcrypt hashing algorithm for a single piece of data
uint256 dcrypt(const uint8_t *data, size_t data_sz, uint8_t *hash_digest = 0);

#endif
