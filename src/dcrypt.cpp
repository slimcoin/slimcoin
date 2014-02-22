#include <stdio.h>
#include <stdlib.h>        //malloc, free, realloc
#include <math.h>          //pow
#include <assert.h>        //for assert

#include "uint256.h"
#include "main.h"
#include "dcrypt.h"

//expand the hash transfer size 0 times
#define EXPAND_HASH_TIMES 0

//the base size for realloc will be 1MB
#define REALLOC_BASE_SZ   (1024 * 1024)

class Extend_Array
{
public:
  Extend_Array()
  {
    //initial values
    array = 0;
    actual_array_sz = 0;
    times_realloced = 0;
  }

  uint8_t *array;
  unsigned long long actual_array_sz;
  uint32_t times_realloced;
};

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

void extend_array(Extend_Array *extend_array, unsigned long long used_array_sz, 
                  uint8_t *extend, uint32_t extend_sz, uint8_t hashed_end)
{
  if(!extend_array)
    return;

  //make a few references
  unsigned long long &actual_array_sz = extend_array->actual_array_sz;
  uint32_t &times_realloced = extend_array->times_realloced;

  //if there is not enough room
  if((actual_array_sz - used_array_sz) < (extend_sz + hashed_end))
  {
    //if extend_array->array has already been malloc'd
    if(times_realloced)
    {
      printf("IP\n");
      //reallocate on an exponential curve, modern computers have plenty ram
      actual_array_sz += pow(2, times_realloced++) * REALLOC_BASE_SZ;
      extend_array->array = (uint8_t*)realloc(extend_array->array, actual_array_sz);
    }else
    {
      //allocate the base size
      actual_array_sz += REALLOC_BASE_SZ;
      times_realloced++;

      extend_array->array = (uint8_t*)malloc(actual_array_sz); //if we have not allocated anything, malloc
    }
  }

  //copy the data to be extended
  memcpy(extend_array->array + used_array_sz, extend, extend_sz);

  if(hashed_end)
  {
    *(extend_array->array + used_array_sz + extend_sz) = 0; //add the final \000 of the whole string array

    //reset the static variables
    actual_array_sz = times_realloced = 0;
  }
  return;
}

void expand_hash(uint8_t *hash, uint8_t *salt, uint8_t *hash_digest, uint32_t expand_times, uint8_t *output)
{
  if(!expand_times)
  {
    memcpy(output, hash, SHA256_LEN);
    return;
  }

  //set all of uint8_t *output to 0x0
  memset(output, 0x0, (expand_times + 1) * SHA256_LEN);

  //copy the hash over to uint8_t *output
  memcpy(output, hash, SHA256_LEN);

  uint32_t i;
  for(i = 0; i < expand_times; i++)
  {
    //get the salt by hashing what is in output
    sha256_to_str(output, (i + 1) * SHA256_LEN, salt, hash_digest);

    //hash uint8_t *output with the salt
    sha256_salt_to_str(output, (i + 1) * SHA256_LEN, salt, SHA256_LEN, hash, hash_digest);

    //append the new hash to the end of output, i + 1 is to offset the original hash added at the beginning
    memcpy(output + (i + 1) * SHA256_LEN, hash, SHA256_LEN);
  }

  return;
}

uint8_t *hash_append(char **str_nums, uint32_t n_str, uint8_t *hash_digest)
{
  uint8_t *appended;
  appended = (uint8_t*)malloc(n_str * SHA256_LEN);

  uint32_t i;
  for(i = 0; i < n_str; i++)
    sha256_to_str((const uint8_t*)*(str_nums + i), strlen(*(str_nums + i)), appended + i * SHA256_LEN, hash_digest);

  return appended;
}

uint32_t rehash_pairs(uint8_t *hashed_nums, uint32_t n_str, uint8_t *hash_digest)
{
  uint32_t hashed_nums_len = n_str * SHA256_LEN;
  uint8_t **new_hash;
  new_hash = (uint8_t**)malloc(sizeof(uint8_t*) * n_str);

  uint32_t i, h = 0, j = 0;

  //allocate for the hashes in new_hash
  for(i = 0; i < n_str; i++)
  {
    *(new_hash + i) = (uint8_t*)malloc(SHA256_LEN + 1);
    *(*(new_hash + i) + SHA256_LEN) = 0; //set the termanating \000
  }

  //split every n_num character in hashed_nums into the separate hashes in new_hash
  for(i = 0; i < hashed_nums_len; i++)
  {
    //reset the counter every n_num interations
    if(!(i % n_str) && i)
    {
      j++;
      h = 0;
    }

    *(*(new_hash + h) + j) = *(hashed_nums + i);
    h++;
    
  }

  //do the actuall rehashing
  for(i = 0; i < n_str; i++)
  {
    sha256_to_str(*(new_hash + i), SHA256_LEN, hashed_nums + i * SHA256_LEN, hash_digest);
    free(*(new_hash + i));
  }

  free(new_hash);

  //sucess!
  return 0;
}

uint64 mix_hashed_nums(uint8_t *hashed_nums, uint32_t n_str, uint8_t **mixed_hash, uint8_t *hash_digest)
{
  uint8_t hashed_end = false;
  uint32_t i, index = 0, hashed_nums_len = n_str * SHA256_LEN;
  uint64 count;
  uint8_t tmp_val, tmp_array[SHA256_LEN + 2];

  //initialize the class for the extend hash
  Extend_Array new_hash;

  //set the first hash length in the temp array to all 0xff
  memset(tmp_array, 0xff, SHA256_LEN);
  //set the last two bytes to \000
  *(tmp_array + SHA256_LEN) = *(tmp_array + SHA256_LEN + 1) = 0;

//only define those arrays/variables if EXPAND_HASH_TIMES != 0
#if EXPAND_HASH_TIMES
  const uint32_t expand_times = EXPAND_HASH_TIMES;
  //expand_times + 1 since if expand = 0, then the expanded_hash will have the same size as the original hash
  const uint32_t expanded_hash_sz = (expand_times + 1) * SHA256_LEN;

  uint8_t expanded_hash[expanded_hash_sz], salt[SHA256_LEN + 1];

#endif

  for(count = 0; hashed_end == false; count++)
  {
    //+1 to keeps a 0 value of *(hashed_nums + index) moving on
    i = hex_char_to_int(*(hashed_nums + index)) + 1;
    index += i;
    
    //if we hit the end of the hash, rehash it
    if(index >= hashed_nums_len)
    {
      index = index % hashed_nums_len;
      rehash_pairs(hashed_nums, n_str, hash_digest);
    }
    
    tmp_val = *(hashed_nums + index);
    join_to_array(tmp_array, tmp_val); //plop tmp_val at the end of tmp_array
    sha256_to_str(tmp_array, SHA256_LEN + 1, tmp_array, hash_digest);

    //check if the last value of hashed_nums is the same as the last value in tmp_array
    if(index == hashed_nums_len - 1)
      if(tmp_val == *(tmp_array + SHA256_LEN - 1))
        hashed_end = true;

//call the expand_hash function only if EXPAND_HASH_TIMES != 0
#if EXPAND_HASH_TIMES
    //expand the hash before extending the array
    expand_hash(tmp_array, salt, hash_digest, expand_times, expanded_hash);

    //extend the expanded hash to the array
    extend_array(&new_hash, count * expanded_hash_sz, expanded_hash, expanded_hash_sz, hashed_end);
#else
    //extend the expanded hash to the array
    extend_array(&new_hash, count * SHA256_LEN, tmp_array, SHA256_LEN, hashed_end);
#endif

  }

  //assign the address of new_hash's array to mixed_hash
  *mixed_hash = new_hash.array;

//these were not created if EXPAND_HASH_TIME == 0
#if EXPAND_HASH_TIMES
  return count * expanded_hash_sz;
#else
  return count * SHA256_LEN;
#endif

}

uint256 dcrypt(char **str_nums, uint32_t n_str, char *out_hash, uint8_t *hash_digest)
{
  uint8_t *hashed_nums, *mix_hash;
  uint256 hash;

  if(!hash_digest)
  {
    uint8_t hdig[DCRYPT_DIGEST_LENGTH];
    hash_digest = hdig;
  }

  //append the hashes
  hashed_nums = hash_append(str_nums, n_str, hash_digest);

  //mix the hashes up, magority of the time takes here
  uint64 mix_hash_len = mix_hashed_nums(hashed_nums, n_str, &mix_hash, hash_digest);

  //apply the final hash to the output
  sha256((const uint8_t*)mix_hash, mix_hash_len, &hash);

  free(hashed_nums);
  free(mix_hash);

  //sucess
  return hash;
}

uint256 dcrypt(const uint8_t *data, size_t data_sz, uint8_t *hash_digest)
{
  uint8_t hashed_nums[SHA256_LEN + 1], *mix_hash;
  uint256 hash;

  if(!hash_digest)
  {
    uint8_t hdig[DCRYPT_DIGEST_LENGTH];
    hash_digest = hdig;
  }

  sha256_to_str(data, data_sz, hashed_nums, hash_digest);

  //mix the hashes up, magority of the time takes here
  uint64 mix_hash_len = mix_hashed_nums(hashed_nums, 1, &mix_hash, hash_digest);

  //apply the final hash to the output
  sha256((const uint8_t*)mix_hash, mix_hash_len, &hash);

  free(mix_hash);

  //sucess
  return hash;
}
