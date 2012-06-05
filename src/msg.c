//#define to access to definition of PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP
// (Linux specific?)
#define _GNU_SOURCE 

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#include "msg.h"


wiw * wagonToSend = NULL;
pthread_mutex_t mutexWagonToSend = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;
pthread_cond_t condWagonToSend;


wagon* firstWagon(Msg * msg){
  if(msg->type==TRAIN){
    if(msg->len == (sizeof(int)+sizeof(MType)+sizeof(stamp)+sizeof(address_set)) ){
      return(msg->body.train.wagons);
    }
    else {
      return(NULL);
    }
  }
  else {
    error_at_line(EXIT_FAILURE,0,__FILE__,__LINE__,"Bad type of message given to firstWagon");
    return(NULL);
  }
}

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

Msg init_msg(){
  Msg msg;

  msg.type=DEFAULT;
  msg.body.def.problem_id=0;
  msg.len=sizeof(msg);
  return msg;
}

Msg newMsg(MType mtype, address addr_id){
  Msg msg=init_msg();

  switch(mtype){
  case DEFAULT:
    break;
  case TRAIN:
    msg.body.def.problem_id=1;
    break;
  case INSERT:
    msg.type=mtype;
    msg.body.insert.sender=addr_id;
    msg.len=sizeof(msg);
    break;
  case ACK_INSERT:
    msg.type=mtype;
    msg.body.ackInsert.sender=addr_id;
    msg.len=sizeof(msg);
    break;
  case NAK_INSERT:
    msg.type=mtype;
    msg.body.nakInsert.sender=addr_id;
    msg.len=sizeof(msg);
    break;
  case NEWSUCC:
    msg.type=mtype;
    msg.body.newSucc.sender=addr_id;
    msg.len=sizeof(msg);
    break;
  case DISCONNECT:
    msg.type=mtype;
    msg.body.disconnect.sender=addr_id;
    msg.len=sizeof(msg);
    break;
  default:
    msg.body.def.problem_id=-1;
    perror("Error creating the message Msg");
    break;
  }

  return(msg);
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
