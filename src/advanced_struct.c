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

bool is_in_lts(address ad, lts_array ltsarray) {
  bool result=true;
  int i;
  for (i=0;i<ntr;i++) {
    result=result & addr_ismember(ad,ltsarray[i].circuit);
  }
  return result;
}

bool is_recent_train(stamp tr_st,lts_array lts, char last_id){
  int waiting_id;
  int diff;

  waiting_id=(last_id+1)%ntr;
  if(tr_st.id==waiting_id){

    //printf("id of the stamp given %d\n",tr_st.id);
    //printf("logical clock of stamp in the is_recent_train %d\n",tr_st.lc);
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

wiw * newwiw(){
  wiw *pp;
  womim *pw;
  pw = malloc(sizeof(prefix)+sizeof(wagon_header));
  assert(pw != NULL);
  pthread_mutex_init(&(pw->pfx.mutex),NULL);
  pw->pfx.counter = 1;
  pw->wagon.header.len = sizeof(wagon_header);
  pw->wagon.header.sender = my_address;
  pw->wagon.header.round = 0;

  pp = malloc(sizeof(wiw));
  assert(pp != NULL);
  pp->p_wagon = &(pw->wagon);
  pp->p_womim = pw;

  return pp;
}

message * mallocwiw(int payloadSize){
  message *mp;
  wagon *w;
  int newWomimLen = sizeof(prefix) + wagonToSend->p_wagon->header.len +
    sizeof(message_header) + payloadSize;
  wagonToSend->p_womim = realloc(wagonToSend->p_womim, newWomimLen);
  assert(wagonToSend->p_womim != NULL);
  w = &(wagonToSend->p_womim->wagon);
  wagonToSend->p_wagon = w;
  mp =(message*)(((char*)w) + w->header.len);
  mp->header.len = sizeof(message_header) + payloadSize;
  w->header.len += mp->header.len;
  return mp;
}

void release_wiw(wiw * ww){
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

void free_wiw(wiw * ww){
  release_wiw(ww);
  free(ww);
}

void free_womim(womim *wo){
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
