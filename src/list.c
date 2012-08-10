/**
 Trains Protocol: Middleware for Uniform and Totally Ordered Broadcasts
 Copyright: Copyright (C) 2010-2012
 Contact: michel.simatic@telecom-sudparis.eu

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 3 of the License, or any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
 USA

 Developer(s): Michel Simatic, Arthur Foltz, Damien Graux, Nicolas Hascoet, Nathan Reboud
 */

#include <error.h>
#include <assert.h>
#include <stdlib.h>

#include "list.h"
#include "common.h"

#define NULLINK ((LINK*)NULL)

trList *newList(){
  trList *aList;
  aList = malloc(sizeof(trList));
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

void *listRemoveFirst(trList *aList){
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

void listAppend(trList *aList, void *anElt){
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

void listExtend(trList* aList, trList* b_list){
  LINK *link;

  // We append the elements of b_list in alist
  for (link = b_list->first; link->value != NULL ; link = link->next){
    listAppend(aList,link->value);
  }
}

//list_clean just remove all the elements from at_list but let it alive
void cleanList(trList* aList){
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

void freeList(trList *aList){
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

