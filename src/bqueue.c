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
#include <errno.h>
#include <assert.h>
#include <stdlib.h>

#include "bqueue.h"

t_bqueue *newBqueue(){
  t_bqueue *aBQueue;

  aBQueue = malloc(sizeof(t_bqueue));
  assert(aBQueue != NULL);

  aBQueue->list = newList();

  if (sem_init(&(aBQueue->readSem),0,0))
    error_at_line(EXIT_FAILURE,errno,__FILE__,__LINE__,"sem_init");

  return aBQueue;
}

void *bqueueDequeue(t_bqueue *aBQueue){
  int rc;

  do {
    rc = sem_wait(&(aBQueue->readSem));
  } while ((rc < 0) && (errno == EINTR));
  if (rc)
    error_at_line(EXIT_FAILURE,errno,__FILE__,__LINE__,"sem_wait");

  return listRemoveFirst(aBQueue->list);
}

void bqueueEnqueue(t_bqueue *aBQueue, void *anElt){
  listAppend(aBQueue->list, anElt);

  if (sem_post(&(aBQueue->readSem)))
    error_at_line(EXIT_FAILURE,errno,__FILE__,__LINE__,"sem_post");
}

void bqueueExtend(t_bqueue *aBQueue, t_list *list){
  LINK *link;

  link=list->first;
  while (link && link->value) {
    bqueueEnqueue(aBQueue, link->value);
    link=link->next;
  }

} 

void freeBqueue(t_bqueue *aBQueue){
  freeList(aBQueue->list);

  if (sem_destroy(&(aBQueue->readSem)))
    error_at_line(EXIT_FAILURE,errno,__FILE__,__LINE__,"sem_destroy");

  free(aBQueue);
}
