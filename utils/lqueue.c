#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>
#include <lqueue.h>
#include <queue.h>


lqueue_t *lqopen(void) {
  lqueue_t *lqptr;

  if (!(lqptr = (lqueue_t *)malloc(sizeof(lqueue_t)))) {
    printf("Error: malloc failed allocating locked queue\n");
    return NULL;
  }

  lqptr -> qptr = qopen();
  pthread_mutex_init(&(lqptr -> m), NULL);

  return (lqueue_t *)lqptr;
}


int32_t lqput(lqueue_t *lqp, void *elementp) {
  lqueue_t *mlqp = (lqueue_t *)lqp;
  pthread_mutex_lock(&(mlqp -> m));

  if (qput(mlqp -> qptr, elementp) != 0) {
    pthread_mutex_unlock(&(mlqp -> m));
    return 1;
  }

  pthread_mutex_unlock(&(mlqp -> m));
  return 0;
}


void *lqget(lqueue_t *lqp) {
  lqueue_t *mlqp = (lqueue_t *)lqp;

  pthread_mutex_lock(&(mlqp -> m));
  void *elementPtr = qget(mlqp -> qptr);
  pthread_mutex_unlock(&(mlqp -> m));

  return elementPtr;
}


void lqapply(lqueue_t *lqp, void (*fn)(void *elementp)) {
  lqueue_t *mlqp = (lqueue_t *)lqp;

  pthread_mutex_lock(&(mlqp -> m));
  qapply(mlqp -> qptr, fn);
  pthread_mutex_unlock(&(mlqp -> m));
}


void *lqsearch(lqueue_t *lqp, bool (*searchfn)(void *elementp, const void *keyp), const void *skeyp) {
  lqueue_t *mlqp = (lqueue_t *)lqp;

  pthread_mutex_lock(&(mlqp -> m));
  void *elementPtr = qsearch(mlqp -> qptr, searchfn, skeyp);
  pthread_mutex_unlock(&(mlqp -> m));

  return elementPtr;
}


void lqclose(lqueue_t *lqp) {
  lqueue_t *mlqp = (lqueue_t *)lqp;

  qclose(mlqp -> qptr);
  pthread_mutex_destroy(&(mlqp -> m));
  free(mlqp);
}


