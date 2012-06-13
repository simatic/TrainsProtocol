#include <error.h>
#include <errno.h>
#include <assert.h>
#include <stdlib.h>

#include "bqueue.h"

t_bqueue *bqueue_new(){
  t_bqueue *aBQueue;

  aBQueue = malloc(sizeof(t_bqueue));
  assert(aBQueue != NULL);

  aBQueue->list = list_new();

  if (sem_init(&(aBQueue->readSem),0,0))
    error_at_line(EXIT_FAILURE,errno,__FILE__,__LINE__,"sem_init");

  return aBQueue;
}

void *bqueue_dequeue(t_bqueue *aBQueue){
  int rc;

  do {
    rc = sem_wait(&(aBQueue->readSem));
  } while ((rc < 0) && (errno == EINTR));
  if (rc)
    error_at_line(EXIT_FAILURE,errno,__FILE__,__LINE__,"sem_wait");

  return list_removeFirst(aBQueue->list);
}

void bqueue_enqueue(t_bqueue *aBQueue, void *anElt){
  list_append(aBQueue->list, anElt);

  if (sem_post(&(aBQueue->readSem)))
    error_at_line(EXIT_FAILURE,errno,__FILE__,__LINE__,"sem_post");
}

void bqueue_extend(t_bqueue *aBQueue, t_list *list){
  LINK *link;

  link=list->first;
  while (link && link->value) {
    bqueue_enqueue(aBQueue, link->value);
    link=link->next;
  }

} 

void bqueue_free(t_bqueue *aBQueue){
  list_free(aBQueue->list);

  if (sem_destroy(&(aBQueue->readSem)))
    error_at_line(EXIT_FAILURE,errno,__FILE__,__LINE__,"sem_destroy");

  free(aBQueue);
}
