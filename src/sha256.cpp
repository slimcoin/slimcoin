#include <openssl/sha.h>   //for the sha256
#include <string.h>        //for strlen

#include "sha256.h"

void digest_to_string(uint8_t *hash_digest, char *string)
{
  register uint8_t tmp_val;

  uint32_t i = 0;
  for(; i < SHA256_DIGEST_LENGTH; i++)
  {
    //format the most (left-most) significant 4bit hex
    tmp_val = *(hash_digest + i) >> 4;

    //add 48 to get the integer into a char
    if(tmp_val <= 9)
      *(string + (i * 2)) = tmp_val + 48;
    else //add 87 to get the integer into the a-f of a hex
      *(string + (i * 2)) = tmp_val + 87;

    //format the least (right-most) significant 4bit hex
    tmp_val = *(hash_digest + i) & 0x0F;

    //add 48 to get the integer into a char
    if(tmp_val <= 9)
      *(string + (i * 2) + 1) = tmp_val + 48;
    else //add 87 to get the integer into the a-f of a hex
      *(string + (i * 2) + 1) = tmp_val + 87;

  }

  //add the termination \000 to the string
  *(string + SHA256_LEN) = 0;
  
  return;
}

void sha256(char *string, char *outputBuffer, uint8_t *hash_digest)
{
  SHA256_CTX sha256;
  SHA256_Init(&sha256);
  SHA256_Update(&sha256, string, strlen(string));
  SHA256_Final(hash_digest, &sha256);

  //convert the digest to a string
  digest_to_string(hash_digest, outputBuffer);

  //sucess!
  return;
}

void sha256_salt(char *string, char *salt, char *outputBuffer, uint8_t *hash_digest)
{
  SHA256_CTX sha256;
  SHA256_Init(&sha256);
  SHA256_Update(&sha256, string, strlen(string));
  SHA256_Update(&sha256, salt, strlen(salt));
  SHA256_Final(hash_digest, &sha256);

  //convert the digest to a string
  digest_to_string(hash_digest, outputBuffer);    

  //sucess!
  return;
}
