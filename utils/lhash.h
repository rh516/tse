#pragma once
/*
 * hash.h -- A generic hash table implementation, allowing arbitrary
 * key structures.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>
#include <lhash.h>
#include <hash.h>

typedef struct lhashtable {
	pthread_mutex_t m;
  hashtable_t *htptr;
}	lhashtable_t;

/* hopen -- opens a hash table with initial size hsize */
lhashtable_t *lhopen(uint32_t hsize);

/* hclose -- closes a hash table */
void lhclose(lhashtable_t *lhtp);

/* hput -- puts an entry into a hash table under designated key 
 * returns 0 for success; non-zero otherwise
 */
int32_t lhput(lhashtable_t *lhtp, void *ep, const char *key, int keylen);

/* happly -- applies a function to every entry in hash table */
void lhapply(lhashtable_t *lhtp, void (*fn)(void* ep));

/* hsearch -- searchs for an entry under a designated key using a
 * designated search fn -- returns a pointer to the entry or NULL if
 * not found
 */
void *lhsearch(lhashtable_t *lhtp, 
	      bool (*searchfn)(void* elementp, const void* searchkeyp), 
	      const char *key, 
	      int32_t keylen);

