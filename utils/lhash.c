#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>
#include <lhash.h>
#include <hash.h>


lhashtable_t *lhopen(uint32_t hsize) {
  lhashtable_t *mlhtp;

  if (!(mlhtp = (lhashtable_t *)malloc(sizeof(lhashtable_t)))) {
    printf("Error: malloc failed allocating locked hash table\n");
    return NULL;
  }

  mlhtp -> htptr = hopen(hsize);
  pthread_mutex_init(&(mlhtp -> m), NULL);

  return mlhtp;
}


int32_t lhput(lhashtable_t *lhtp, void *ep, const char *key, int keylen) {
  pthread_mutex_lock(&(lhtp -> m));

  if (hput(lhtp -> htptr, ep, key, keylen) != 0) {
    pthread_mutex_unlock(&(lhtp -> m));
    return 1;
  }

  pthread_mutex_unlock(&(lhtp -> m));
  return 0;  
}


void lhapply(lhashtable_t *lhtp, void (*fn)(void* ep)) {
  pthread_mutex_lock(&(lhtp -> m));
  happly(lhtp -> htptr, fn);
  pthread_mutex_unlock(&(lhtp -> m));
}


void *lhsearch(lhashtable_t *lhtp, 
	            bool (*searchfn)(void* elementp, const void* searchkeyp), 
	            const char *key, 
	            int32_t keylen) 
{
  pthread_mutex_lock(&(lhtp -> m));
  void *elementPtr = hsearch(lhtp -> htptr, searchfn, key, keylen);
  pthread_mutex_unlock(&(lhtp -> m));

  return elementPtr;
}


void lhclose(lhashtable_t *lhtp) {
  hclose(lhtp -> htptr);
  pthread_mutex_destroy(&(lhtp -> m));
  free(lhtp);
}


