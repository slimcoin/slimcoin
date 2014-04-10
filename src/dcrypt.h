
#ifndef DCRYPT_H
#define DCRYPT_H

#include "sha256.h"

#define DCRYPT_DIGEST_LENGTH SHA256_DIGEST_LENGTH //value: 20
#define DCRYPT_MIX_HASH_LEN (SHA256_LEN + 1)      //value: (64 + 1) = 65

//the dcrypt hashing algorithm for a single piece of data
uint256 dcrypt(const uint8_t *data, size_t data_sz, uint8_t *hash_digest = 0);

//specialized dcrypt used for mining, (aka: batteries not included)
uint256 dcrypt_sp(const uint8_t *data, size_t data_sz, uint8_t *dcrypt_mix_hash, 
                  uint8_t *hash_digest, std::vector<u8int> &vMixedHash);

#endif
