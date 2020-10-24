/* 
 * hash.c -- implements a generic hash table as an indexed set of queues.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "hash.h"
#include "queue.h"

typedef struct hashT
{
  uint32_t size;
  queue_t **queuesPtr;
} myHashT;

hashtable_t *hopen(uint32_t hsize)
{
  myHashT *hp;

  if (!(hp = (myHashT *)malloc(sizeof(myHashT))))
  {
    printf("Error: malloc failed allocating hash table\n");
    return NULL;
  }

  hp->size = hsize;
  hp->queuesPtr = (queue_t **)malloc(sizeof(queue_t *) * hsize);

  if (hp->queuesPtr == NULL)
  {
    free(hp);
    printf("failed to malloc array of size %d\n", hsize);
  }

  uint32_t i;
  for (i = 0; i < hsize; i++)
  {
    (hp->queuesPtr)[i] = qopen();

    if ((hp->queuesPtr)[i] == NULL)
    {
      while (i >= 0)
      {
        free((hp->queuesPtr)[i]);
        i--;
      }
      free(hp->queuesPtr);
      free(hp);
      printf("failed to malloc queue at index %d\n", i);
    }
  }

  return (hashtable_t *)hp;
}

void hclose(hashtable_t *htp)
{
  if (htp != NULL)
  {
    myHashT *mhtp = (myHashT *)htp;

    uint32_t i;
    if (mhtp->queuesPtr != NULL)
    {
      for (i = 0; i < (mhtp->size); i++)
      {
        if ((mhtp->queuesPtr)[i] != NULL)
        {
          qclose((mhtp->queuesPtr)[i]);
        }
      }
      free(mhtp->queuesPtr);
    }

    free(mhtp);
  }
}

void happly(hashtable_t *htp, void (*fn)(void *ep))
{
  if (htp != NULL)
  {
    myHashT *mhtp = (myHashT *)htp;

    uint32_t i;
    if (mhtp->queuesPtr != NULL)
    {
      for (i = 0; i < (mhtp->size); i++)
      {
        if ((mhtp->queuesPtr)[i] != NULL)
        {
          qapply((mhtp->queuesPtr)[i], fn);
        }
      }
    }
  }
}

/* 
 * SuperFastHash() -- produces a number between 0 and the tablesize-1.
 * 
 * The following (rather complicated) code, has been taken from Paul
 * Hsieh's website under the terms of the BSD license. It's a hash
 * function used all over the place nowadays, including Google Sparse
 * Hash.
 */
#define get16bits(d) (*((const uint16_t *)(d)))

static uint32_t SuperFastHash(const char *data, int len, uint32_t tablesize)
{
  uint32_t hash = len, tmp;
  int rem;

  if (len <= 0 || data == NULL)
    return 0;
  rem = len & 3;
  len >>= 2;
  /* Main loop */
  for (; len > 0; len--)
  {
    hash += get16bits(data);
    tmp = (get16bits(data + 2) << 11) ^ hash;
    hash = (hash << 16) ^ tmp;
    data += 2 * sizeof(uint16_t);
    hash += hash >> 11;
  }
  /* Handle end cases */
  switch (rem)
  {
  case 3:
    hash += get16bits(data);
    hash ^= hash << 16;
    hash ^= data[sizeof(uint16_t)] << 18;
    hash += hash >> 11;
    break;
  case 2:
    hash += get16bits(data);
    hash ^= hash << 11;
    hash += hash >> 17;
    break;
  case 1:
    hash += *data;
    hash ^= hash << 10;
    hash += hash >> 1;
  }
  /* Force "avalanching" of final 127 bits */
  hash ^= hash << 3;
  hash += hash >> 5;
  hash ^= hash << 4;
  hash += hash >> 17;
  hash ^= hash << 25;
  hash += hash >> 6;
  return hash % tablesize;
}

int32_t hput(hashtable_t *htp, void *ep, const char *key, int keylen)
{
  if (ep == NULL)
  {
    printf("Can't put null element\n");
    return 1;
  }

  if (htp == NULL)
  {
    printf("Hash table can't be null\n");
    return 1;
  }

  myHashT *mhtp = (myHashT *)htp;
  uint32_t hashIdx = SuperFastHash(key, keylen, mhtp->size);

  if (mhtp->queuesPtr == NULL)
  {
    printf("Array can't be null\n");
    return 1;
  }

  if ((mhtp->queuesPtr)[hashIdx] != NULL)
  {
    qput((mhtp->queuesPtr)[hashIdx], ep);
  }
  else
  {
    printf("Queue can't be null\n");
    return 1;
  }

  return 0;
}

/* hsearch -- searchs for an entry under a designated key using a
 * designated search fn -- returns a pointer to the entry or NULL if
 * not found
 */
void *hsearch(hashtable_t *htp,
              bool (*searchfn)(void *elementp, const void *searchkeyp),
              const char *key,
              int32_t keylen)
{
  if (htp == NULL || key == NULL || keylen <= 0 || searchfn == NULL)
  {
    printf("Invalid parameters\n");
    return NULL;
  }
  myHashT *mhtp = (myHashT *)htp;
  uint32_t hashIdx = SuperFastHash(key, keylen, mhtp->size);
  return qsearch(mhtp->queuesPtr[hashIdx], searchfn, key);
}
/* hremove -- removes and returns an entry under a designated key
 * using a designated search fn -- returns a pointer to the entry or
 * NULL if not found
 */
void *hremove(hashtable_t *htp,
              bool (*searchfn)(void *elementp, const void *searchkeyp),
              const char *key,
              int32_t keylen)
{
  if (htp == NULL || key == NULL || keylen <= 0 || searchfn == NULL)
  {
    printf("Invalid perameters\n");
    return NULL;
  }
  myHashT *mhtp = (myHashT *)htp;
  uint32_t hashIdx = SuperFastHash(key, keylen, mhtp->size);
  return qremove(mhtp->queuesPtr[hashIdx], searchfn, key);
}
