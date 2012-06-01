#include <stdlib.h>
#include <stdio.h>

#include "address.h"
#include "msg.h"


wagon* nextWagon (Msg_extended* msg_ext, wagon* w) {
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

//FIXME -> change free
void free_wagon(wagon_watcher ww){
  pthread_mutex_lock(&(ww.p_pfx->mutex)); //mutex locked
  ww.p_pfx->counter --; //decrementation of the counter
  if (ww.p_pfx->counter <= 0)
    free(ww.p_wagon);//FIXME -> help help
  pthread_mutex_unlock(&(ww.p_pfx->mutex)); //mutex unlocked
  free(ww.p_pfx);
  free(&ww);
}
//free((char[length+sizeof(pthred_mutex)+sizeof(int)])ww.p_pfx)


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
