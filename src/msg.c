//#define to access to definition of PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP
// (Linux specific?)
#define _GNU_SOURCE 

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#include "msg.h"




wagon* firstWagon(Msg * msg){
  if(msg->type==TRAIN){
    if(msg->len > (sizeof(int)+sizeof(MType)+sizeof(stamp)+sizeof(address_set)) ){
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
    msg.len=sizeof(int)+sizeof(MType)+sizeof(address);
    break;
  case ACK_INSERT:
    msg.type=mtype;
    msg.body.ackInsert.sender=addr_id;
    msg.len=sizeof(int)+sizeof(MType)+sizeof(address);;
    break;
  case NAK_INSERT:
    msg.type=mtype;
    msg.body.nakInsert.sender=addr_id;
    msg.len=sizeof(int)+sizeof(MType)+sizeof(address);;
    break;
  case NEWSUCC:
    msg.type=mtype;
    msg.body.newSucc.sender=addr_id;
    msg.len=sizeof(int)+sizeof(MType)+sizeof(address);;
    break;
  case DISCONNECT:
    msg.type=mtype;
    msg.body.disconnect.sender=addr_id;
    msg.len=sizeof(int)+sizeof(MType)+sizeof(address);;
    break;
  default:
    msg.body.def.problem_id=-1;
    perror("Error creating the message Msg");
    break;
  }

  return(msg);
}

