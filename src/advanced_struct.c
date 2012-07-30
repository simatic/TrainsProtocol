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

//#define to access to definition of PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP
// (Linux specific?)
#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#include "advanced_struct.h"
#include "common.h"

wiw * wagonToSend = NULL;
pthread_mutex_t mutexWagonToSend = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;
pthread_cond_t condWagonToSend;



wagon* nextWagon (womim* msg_ext, wagon* w) {
  wagon *w2= (wagon*)((char*)w+(w->header.len));
  if ((char*)w2 - (char*)(&(msg_ext->msg)) >= msg_ext->msg.len)
    return NULL;
  else
    return w2;
}

bool isInLts(address ad, ltsArray ltsarray) {
  bool result=true;
  int i;
  for (i=0;i<ntr;i++) {
    result=result & addrIsMember(ad,ltsarray[i].circuit);
  }
  return result;
}

bool isRecentTrain(stamp tr_st,ltsArray lts, char last_id){
  int waiting_id;
  int diff;

  waiting_id=(last_id+1)%ntr;
  if(tr_st.id==waiting_id){

    //printf("id of the stamp given %d\n",tr_st.id);
    //printf("logical clock of stamp in the isRecentTrain %d\n",tr_st.lc);
    //printf("logical clock in lts %d\n",lts[waiting_id].stamp.lc);

    diff=tr_st.lc - lts[waiting_id].stamp.lc;
    if(diff>0)
      return(diff<((1+M)/2));
    else{ 
      return(diff<((1-M)/2));
    }
  }
  else{
    return(false);
  }
}

wiw * newWiw(){
  wiw *pp;
  womim *pw;
  pw = malloc(sizeof(prefix)+sizeof(wagonHeader));
  assert(pw != NULL);
  pthread_mutex_init(&(pw->pfx.mutex),NULL);
  pw->pfx.counter = 1;
  pw->wagon.header.len = sizeof(wagonHeader);
  pw->wagon.header.sender = myAddress;
  pw->wagon.header.round = 0;

  pp = malloc(sizeof(wiw));
  assert(pp != NULL);
  pp->p_wagon = &(pw->wagon);
  pp->p_womim = pw;

  return pp;
}

message * mallocWiw(int payloadSize){
  message *mp;
  wagon *w;
  int newWomimLen = sizeof(prefix) + wagonToSend->p_wagon->header.len +
    sizeof(messageHeader) + payloadSize;
  fprintf(stderr, "reallocation");
  wagonToSend->p_womim = realloc(wagonToSend->p_womim, newWomimLen);
  assert(wagonToSend->p_womim != NULL);
  w = &(wagonToSend->p_womim->wagon);
  wagonToSend->p_wagon = w;
  mp =(message*)(((char*)w) + w->header.len);
  mp->header.len = sizeof(messageHeader) + payloadSize;
  w->header.len += mp->header.len;
  return mp;
}

void releaseWiw(wiw * ww){
  if((ww !=NULL) && (ww->p_womim != NULL)){
    MUTEX_LOCK(ww->p_womim->pfx.mutex);
    ww->p_womim->pfx.counter -= 1;
    if (ww->p_womim->pfx.counter == 0) {
      MUTEX_UNLOCK(ww->p_womim->pfx.mutex);
      MUTEX_DESTROY(ww->p_womim->pfx.mutex);
      free(ww->p_womim);
    }
    else {
      MUTEX_UNLOCK(ww->p_womim->pfx.mutex);
    }
    ww->p_wagon = NULL;
    ww->p_womim = NULL;
  }
}

void freeWiw(wiw * ww){
  releaseWiw(ww);
  free(ww);
}

void freeWomim(womim *wo){
  MUTEX_LOCK(wo->pfx.mutex);
  wo->pfx.counter--;
  if (wo->pfx.counter == 0) {
    MUTEX_UNLOCK(wo->pfx.mutex);
    MUTEX_DESTROY(wo->pfx.mutex);
    free(wo);
  } else {
    MUTEX_UNLOCK(wo->pfx.mutex);
  }
}
