#include <stdlib.h>
#include <stdio.h>

#include "address.h"
#include "msg.h"
//FIXME -> What about train structure [same issue in .h]

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
