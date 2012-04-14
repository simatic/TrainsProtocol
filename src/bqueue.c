#include <error.h>
#include <errno.h>
#include <assert.h>
#include <stdlib.h>

#include "bqueue.h"

t_bqueue *bqueue_new(){
  t_bqueue *aBQueue;
  int rc;

  aBQueue = malloc(sizeof(t_bqueue));
  assert(aBQueue != NULL);

  aBQueue->list = list_new();

  rc = sem_init(&(aBQueue->readSem),0,0);
  if (rc)
    error_at_line(EXIT_FAILURE,rc,__FILE__,__LINE__,"sem_init");

  return aBQueue;
}

void *bqueue_dequeue(t_bqueue *aBQueue){
  int rc;

  do {
    rc = sem_wait(&(aBQueue->readSem));
  } while ((rc < 0) && (errno == EINTR));
  if (rc)
    error_at_line(EXIT_FAILURE,rc,__FILE__,__LINE__,"sem_wait");

  return list_removeFirst(aBQueue->list);
}

void bqueue_enqueue(t_bqueue *aBQueue, void *anElt){
  int rc;

  list_append(aBQueue->list, anElt);

  rc = sem_post(&(aBQueue->readSem));
  if (rc)
    error_at_line(EXIT_FAILURE,rc,__FILE__,__LINE__,"sem_post");
}

void bqueue_free(t_bqueue *aBQueue){
  int rc;

  list_free(aBQueue->list);

  rc = sem_destroy(&(aBQueue->readSem));
  if (rc)
    error_at_line(EXIT_FAILURE,rc,__FILE__,__LINE__,"sem_destroy");

  free(aBQueue);
}
