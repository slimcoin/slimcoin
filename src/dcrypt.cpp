#include <stdio.h>
#include <stdlib.h>        //malloc, free, realloc
#include <math.h>          //pow
#include <assert.h>        //for assert

#include "dcrypt.h"

//expand the hash transfer size 0 times
#define EXPAND_HASH_TIMES 0

//the base size for realloc will be 1MB
#define REALLOC_BASE_SZ   (1024 * 1024)

uint32_t hex_char_to_int(char c)
{
  if(c >= '0' && c <= '9')
    return c - '0';

  if(c >= 'a' && c <= 'f')
    return 10 + c - 'a';

  if(c >= 'A' && c <= 'F')
    return 10 + c - 'A';

  return -1;
}

void join_to_array(char *array, char join)
{
  *(array + SHA256_LEN) = join;
  return;
}

void extend_array(char **array, unsigned long long used_array_sz, 
                  char *extend, uint32_t extend_sz, uint8_t hashed_end)
{
  if(!array)
    return;

  static unsigned long long actual_array_sz = 0;
  static uint32_t times_realloced = 0;

  //~ printf("Wow %lld < %d times realloc'd %d\n", (*actual_array_sz - used_array_sz), 
         //~ (extend_sz + hashed_end), *times_realloced);

  //if there is not enough room
  if((actual_array_sz - used_array_sz) < (extend_sz + hashed_end))
  {
    //if *array has already been malloc'd
    if(times_realloced)
    {
      printf("IP\n");
      //reallocate on an exponential curve, modern computers have plenty ram
      actual_array_sz += pow(2, times_realloced++) * REALLOC_BASE_SZ;
      *array = (char*)realloc(*array, actual_array_sz);
    }else
    {
      //allocate the base size
      actual_array_sz += REALLOC_BASE_SZ;
      times_realloced++;

      *array = (char*)malloc(actual_array_sz); //if we have not allocated anything, malloc
    }
  }

  //copy the data to be extended
  memcpy(*array + used_array_sz, extend, extend_sz);

  if(hashed_end)
  {
    *(*array + used_array_sz + extend_sz) = 0; //add the final \000 of the whole string array

    //reset the static variables
    actual_array_sz = times_realloced = 0;
  }
  return;
}

void expand_hash(char *hash, char *salt, uint8_t *hash_digest, uint32_t expand_times, char *output)
{
  if(!expand_times)
  {
    memcpy(output, hash, SHA256_LEN);
    return;
  }

  //set all of char *output to 0x0
  memset(output, 0x0, (expand_times + 1) * SHA256_LEN);

  //copy the hash over to char *output
  memcpy(output, hash, SHA256_LEN);

  uint32_t i;
  for(i = 0; i < expand_times; i++)
  {
    //get the salt by hashing what is in output
    sha256(output, salt, hash_digest);

    //hash char *output with the salt
    sha256_salt(output, salt, hash, hash_digest);

    //append the new hash to the end of output, i + 1 is to offset the original hash added at the beginning
    memcpy(output + (i + 1) * SHA256_LEN, hash, SHA256_LEN);
  }

  return;
}

char *hash_append(char **str_nums, uint32_t n_str, uint8_t *hash_digest)
{
  char *appended;
  appended = (char*)malloc(n_str * SHA256_LEN);

  uint32_t i;
  for(i = 0; i < n_str; i++)
    sha256(*(str_nums + i), appended + i * SHA256_LEN, hash_digest);

  return appended;
}

uint32_t rehash_pairs(char *hashed_nums, uint32_t n_str, uint8_t *hash_digest)
{
  uint32_t hashed_nums_len = n_str * SHA256_LEN;
  char **new_hash;
  new_hash = (char**)malloc(sizeof(char*) * n_str);

  uint32_t i, h = 0, j = 0;

  //allocate for the hashes in new_hash
  for(i = 0; i < n_str; i++)
  {
    *(new_hash + i) = (char*)malloc(SHA256_LEN + 1);
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
    sha256(*(new_hash + i), hashed_nums + i * SHA256_LEN, hash_digest);
    free(*(new_hash + i));
  }

  free(new_hash);

  //sucess!
  return 0;
}

char *mix_hashed_nums(char *hashed_nums, uint32_t n_str, uint8_t *hash_digest)
{
  uint8_t hashed_end = false;
  uint32_t i, index = 0, hashed_nums_len = n_str * SHA256_LEN;
  unsigned long long count;
  char tmp_val, *tmp_array, *new_hash = 0;
  tmp_array = (char*)calloc(SHA256_LEN + 2, sizeof(char)); // +1 for the joining char and +1 for the \000

//only define those arrays/variables if EXPAND_HASH_TIMES != 0
#if EXPAND_HASH_TIMES
  char *expanded_hash, *salt;
  const uint32_t expand_times = EXPAND_HASH_TIMES;
  //expand_times + 1 since if expand = 0, then the expanded_hash will have the same size as the original hash
  const uint32_t expanded_hash_sz = (expand_times + 1) * SHA256_LEN;
  expanded_hash = (char*)malloc(expanded_hash_sz);

  salt = (char*)malloc(SHA256_LEN + 1); // +1 for the \000
#endif

  for(count = 0; hashed_end == false; count++)
  {
    i = hex_char_to_int(*(hashed_nums + index)) + 1;
    index += i;
    
    //if we hit the end of the hash, rehash it
    if(index >= hashed_nums_len)
    {
      index = index % hashed_nums_len;
      rehash_pairs(hashed_nums, n_str, hash_digest);
    }
    
    tmp_val = *(hashed_nums + index);
    join_to_array(tmp_array, tmp_val);
    sha256(tmp_array, tmp_array, hash_digest);

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

  //~ printf("count %lld, %d Mil\n", count, (uint32_t)(count / 1000000));

  //~ long long bytes = strlen(new_hash) + 1;
  //~ printf("Size of final: %lld bytes %lld KB %f MB |||||\n", bytes, bytes / 1024, ((float)bytes) / (1024 * 1024));


  free(tmp_array);

//these were not created if EXPAND_HASH_TIME == 0
#if EXPAND_HASH_TIMES
  free(expanded_hash);
  free(salt);
#endif

  return new_hash;
}

uint32_t dcrypt(char **str_nums, uint32_t n_str, char *out_hash, uint8_t *hash_digest)
{
  char *hashed_nums, *mix_hash;

  //append the hashes
  hashed_nums = hash_append(str_nums, n_str, hash_digest);

  //~ printf("DONE with hash_append\n");

  //mix the hashes up, magority of the time takes here
  mix_hash = mix_hashed_nums(hashed_nums, n_str, hash_digest);

  //~ printf("DONE with mix_hash");

  //apply the final hash to the output
  sha256(mix_hash, out_hash, hash_digest);

  //~ printf(" Done with final hash!\n");

  free(hashed_nums);
  free(mix_hash);

  //sucess
  return 0;
}

uint32_t dcrypt(char *str_nums, char *out_hash, uint8_t *hash_digest)
{
  char *hashed_nums, *mix_hash;

  hashed_nums = (char*)malloc(SHA256_LEN + 1);
  sha256(str_nums, hashed_nums, hash_digest);

  //~ printf("DONE with hash_append\n");

  //mix the hashes up, magority of the time takes here
  mix_hash = mix_hashed_nums(hashed_nums, 1, hash_digest);

  //~ printf("DONE with mix_hash");

  //apply the final hash to the output
  sha256(mix_hash, out_hash, hash_digest);

  //~ printf(" Done with final hash!\n");

  free(hashed_nums);
  free(mix_hash);

  //sucess
  return 0;
}
