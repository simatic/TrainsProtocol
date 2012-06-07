//#define to access to definition of PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP
// (Linux specific?)
#define _GNU_SOURCE

#include <stdlib.h>
#include <pthread.h>

#include "advanced_struct.h"

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

bool is_in_lts(address  ad, lts_array ltsarray) {
  bool result=false;
  int i;
  for (i=0;i<ntr;i++) {
    result=result && addr_ismember(ad,ltsarray[i].circuit);
  }
  return result;
}

bool is_recent_train(stamp tr_st,lts_array * plts_array, char last_id, int nb_train){
  int waiting_id;
  int diff;

  waiting_id=(last_id++)%ntr;
  if(tr_st.id==waiting_id){
    diff=tr_st.lc - ((*plts_array)[waiting_id]).stamp.lc;
    if(diff>0)
      return(diff<((1+NP)/2));
    else{ return(diff<((1-NP)/2)); }
  }
  else{ return(false); }
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

message * mallocwiw(wiw **pw, int payloadSize){
  message *mp;
  wagon *w;
  int newWomimLen = sizeof(prefix) + (*pw)->p_wagon->header.len +
    sizeof(message_header) + payloadSize;
  (*pw)->p_womim = realloc((*pw)->p_womim, newWomimLen);
  assert((*pw)->p_womim != NULL);
  w = (*pw)->p_wagon;
  (*pw)->p_wagon = w;
  mp =(message*)(((char*)w) + w->header.len);
  mp->header.len = sizeof(message_header) + payloadSize;
  w->header.len = newWomimLen;
  return mp;
}

void free_wiw(wiw * ww){
  pthread_mutex_lock(&(ww->p_womim->pfx.mutex));
  ww->p_womim->pfx.counter -= 1;
  if (ww->p_womim->pfx.counter == 0) {
    pthread_mutex_unlock(&(ww->p_womim->pfx.mutex));
    pthread_mutex_destroy(&(ww->p_womim->pfx.mutex));
    free(ww->p_womim);
  }
  else {
    pthread_mutex_unlock(&(ww->p_womim->pfx.mutex));
  }
  free(ww);
}

