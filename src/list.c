#include <error.h>
#include <assert.h>
#include <stdlib.h>

#include "list.h"
#include "common.h"

#define NULLINK ((LINK*)NULL)

t_list *list_new(){
  t_list *aList;
  aList = malloc(sizeof(t_list));
  assert(aList != NULL);
  pthread_mutex_init(&(aList->listMutex),NULL);
  // The list always contains an element. If the list is empty, this element
  // is NULL
  aList->first = malloc(sizeof(LINK));
  assert(aList->first != NULL);
  aList->last = aList->first;
  aList->last->value = NULL;
  aList->last->next = NULLINK;
  aList->last->previous = NULLINK;
  return aList;
}

void *list_removeFirst(t_list *aList){
  LINK *link;
  void *val = NULL;

  MUTEX_LOCK(aList->listMutex);
  if (aList->first && aList->first->value) {
    link = aList->first;
    link->next->previous = NULLINK;
    aList->first = link->next;
    val = link->value;
    free(link);
  }
  MUTEX_UNLOCK(aList->listMutex);
  return val;
}

void list_append(t_list *aList, void *anElt){
  LINK *link, *exLast;

  if (!anElt)
    return;

  link = malloc(sizeof(LINK));
  assert(link != NULL);
  MUTEX_LOCK(aList->listMutex);
  link->next = NULLINK;
  link->value = NULL;
  exLast = aList->last;
  aList->last = link;

  exLast->next = link;
  link->previous = exLast;
  exLast->value = anElt;
  MUTEX_UNLOCK(aList->listMutex);
}

void list_extend(t_list* aList, t_list* b_list){
  LINK *link;

  // We append the elements of b_list in alist
  for (link = b_list->first; link->value != NULL ; link = link->next){
    list_append(aList,link->value);
  }
}

//list_clean just remove all the elements from at_list but let it alive
void list_cleanList(t_list* aList){
  LINK *link, *next;
  // We free all the LINK elements which value is not NULL
  MUTEX_LOCK(aList->listMutex);
  link = aList->first;
  while (link->value != NULL) {
    next = link->next;
    free(link);
    link = next;
  };
  // We update a_list pointers
  aList->first = link;
  aList->last  = link;
  MUTEX_UNLOCK(aList->listMutex);
}

void list_free(t_list *aList){
  LINK *link, *next;

  // We free the remaining LINK elements in the list
  link = aList->first;
  do{
    next = link->next;
    free(link);
    link = next;
  }while(link != NULL);

  // We free the mutex
  int rc = pthread_mutex_destroy(&(aList->listMutex));
  if (rc < 0)
    error_at_line(EXIT_FAILURE,rc,__FILE__,__LINE__,"pthread_mutex_destroy");

  free(aList);
}

