#include <stdio.h>
#include <stdlib.h>        //malloc, free, realloc
#include <math.h>          //pow
#include <assert.h>        //for assert

#include "uint256.h"
#include "main.h"
#include "dcrypt.h"

//the base size for realloc will be 1MB
#define REALLOC_BASE_SZ   (1024 * 1024)

using namespace std;

uint32_t hex_char_to_int(uint8_t c)
{
  if(c >= '0' && c <= '9')
    return c - '0';

  if(c >= 'a' && c <= 'f')
    return 10 + c - 'a';

  if(c >= 'A' && c <= 'F')
    return 10 + c - 'A';

  return -1;
}

inline void join_to_array(uint8_t *array, uint8_t join)
{
  *(array + SHA256_LEN) = join;
  return;
}

uint64 mix_hashed_nums(uint8_t *hashed_nums, vector<u8int> &mixed_hash, uint8_t *hash_digest)
{
  uint32_t i, index = 0;
  const u32int hashed_nums_len = SHA256_LEN;
  uint8_t tmp_val, tmp_array[SHA256_LEN + 1];

  bool hashed_end = false;
  uint64 count;

  //set the first hash length in the temp array to all 0xff
  memset(tmp_array, 0xff, SHA256_LEN);
  //set the last byte to \000
  *(tmp_array + SHA256_LEN) = 0;

  for(count = 0; hashed_end == false; count++)
  {
    //+1 to keeps a 0 value of *(hashed_nums + index) moving on
    i = hex_char_to_int(*(hashed_nums + index)) + 1;
    index += i;
    
    //if we hit the end of the hash, rehash it
    if(index >= hashed_nums_len)
    {
      index = index % hashed_nums_len;

      //rehash the hashed_nums
      sha256_to_str(hashed_nums, SHA256_LEN, hashed_nums, hash_digest);
    }
    
    tmp_val = *(hashed_nums + index);
    join_to_array(tmp_array, tmp_val); //plop tmp_val at the end of tmp_array
    sha256_to_str(tmp_array, SHA256_LEN + 1, tmp_array, hash_digest);

    //check if the last value of hashed_nums is the same as the last value in tmp_array
    if(index == hashed_nums_len - 1)
      if(tmp_val == *(tmp_array + index))
        hashed_end = true;

    //push back the entire tmp_array to the end of mixed_hash
    mixed_hash.insert(mixed_hash.end(), tmp_array, tmp_array + SHA256_LEN);

  }

  return count * SHA256_LEN;

}

uint256 dcrypt(const uint8_t *data, size_t data_sz, uint8_t *hash_digest)
{
  uint8_t hashed_nums[SHA256_LEN + 1];
  vector<u8int> mix_hash;
  uint256 hash;

  static uint8_t hdig[DCRYPT_DIGEST_LENGTH];
  if(!hash_digest)
    hash_digest = hdig;

  sha256_to_str(data, data_sz, hashed_nums, hash_digest);

  //mix the hashes up, magority of the time takes here
  uint64 mix_hash_len = mix_hashed_nums(hashed_nums, mix_hash, hash_digest);

  //apply the final hash to the output
  sha256((const uint8_t*)mix_hash.data(), mix_hash_len, &hash);

  //sucess
  return hash;
}

//specialized dcrypt used for mining, (aka: batteries not included)
uint256 dcrypt_sp(const uint8_t *data, size_t data_sz, uint8_t *dcrypt_mix_hash, 
                  uint8_t *hash_digest, vector<u8int> &vMixedHash)
{
  uint256 hash;

  sha256_to_str(data, data_sz, dcrypt_mix_hash, hash_digest);

  //mix the hashes up, magority of the time takes here
  uint64 mix_hash_len = mix_hashed_nums(dcrypt_mix_hash, vMixedHash, hash_digest);

  //apply the final hash to the output
  sha256((const uint8_t*)vMixedHash.data(), mix_hash_len, &hash);

  //sucess
  return hash;  
}
