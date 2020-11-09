#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>
#include <lhash.h>
#include <hash.h>

typedef struct lhash {
  pthread_mutex_t m;
  hashtable_t *htptr;
} mylhash_t;


lhashtable_t *lhopen(uint32_t hsize) {
  mylhash_t *mlhtp;

  if (!(mlhtp = (mylhash_t *)malloc(sizeof(mylhash_t)))) {
    printf("Error: malloc failed allocating locked hash table\n");
    return NULL;
  }

  mlhtp -> htptr = hopen(hsize);
  pthread_mutex_init(&(mlhtp -> m), NULL);

  return (lhashtable_t *)mlhtp;
}


int32_t lhput(lhashtable_t *lhtp, void *ep, const char *key, int keylen) {
  mylhash_t *mlhtp = (mylhash_t *)lhtp;
  pthread_mutex_lock(&(mlhtp -> m));

  if (hput(mlhtp -> htptr, ep, key, keylen) != 0) {
    pthread_mutex_unlock(&(mlhtp -> m));
    return 1;
  }

  pthread_mutex_unlock(&(mlhtp -> m));
  return 0;  
}


void lhapply(lhashtable_t *lhtp, void (*fn)(void* ep)) {
  mylhash_t *mlhtp = (mylhash_t *)lhtp;

  pthread_mutex_lock(&(mlhtp -> m));
  happly(mlhtp -> htptr, fn);
  pthread_mutex_unlock(&(mlhtp -> m));
}


void *lhsearch(lhashtable_t *lhtp, 
	            bool (*searchfn)(void* elementp, const void* searchkeyp), 
	            const char *key, 
	            int32_t keylen) 
{
  mylhash_t *mlhtp = (mylhash_t *)lhtp;

  pthread_mutex_lock(&(mlhtp -> m));
  void *elementPtr = hsearch(mlhtp -> htptr, searchfn, key, keylen);
  pthread_mutex_unlock(&(mlhtp -> m));

  return elementPtr;
}


void lhclose(lhashtable_t *lhtp) {
  mylhash_t *mlhtp = (mylhash_t *)lhtp;

  hclose(mlhtp -> htptr);
  pthread_mutex_destroy(&(mlhtp -> m));
  free(mlhtp);
}


