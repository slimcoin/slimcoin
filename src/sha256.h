

#ifndef SHA256_H
#define SHA256_H

#include "init.h"
 
#define SHA256_LEN           64 //64 bytes the for the hash itself

//the intermediate sha256 hashing algoritm
void sha256(char *string, char *outputBuffer, uint8_t *hash_digest);
void sha256_salt(char *string, char *salt, char *outputBuffer, uint8_t *hash_digest);

#endif
