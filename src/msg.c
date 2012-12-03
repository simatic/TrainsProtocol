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

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#include "msg.h"




wagon* firstWagon(Msg * msg){
  if(msg->type==TRAIN){
    if(msg->len > (sizeof(int)+sizeof(MType)+sizeof(stamp)+sizeof(addressSet)) ){
      return(msg->body.train.wagons);
    }
    else {
      return(NULL);
    }
  }
  else {
    ERROR_AT_LINE(EXIT_FAILURE,0,__FILE__,__LINE__,"Bad type of message given to firstWagon");
    return(NULL);
  }
}

Msg initMsg(){
  Msg msg;

  msg.type=DEFAULT;
  msg.body.def.problem_id=0;
  msg.len=sizeof(msg);
  return msg;
}

Msg newMsg(MType mtype, address addrID){
  Msg msg=initMsg();

  switch(mtype){
  case DEFAULT:
    break;
  case TRAIN:
    msg.body.def.problem_id=1;
    break;
  case INSERT:
    msg.type=mtype;
    msg.body.insert.sender=addrID;
    msg.len=sizeof(int)+sizeof(MType)+sizeof(address);
    break;
  case ACK_INSERT:
    msg.type=mtype;
    msg.body.ackInsert.sender=addrID;
    msg.len=sizeof(int)+sizeof(MType)+sizeof(address);;
    break;
  case NAK_INSERT:
    msg.type=mtype;
    msg.body.nakInsert.sender=addrID;
    msg.len=sizeof(int)+sizeof(MType)+sizeof(address);;
    break;
  case NEWSUCC:
    msg.type=mtype;
    msg.body.newSucc.sender=addrID;
    msg.len=sizeof(int)+sizeof(MType)+sizeof(address);;
    break;
  case DISCONNECT_PRED:
  case DISCONNECT_SUCC:
    msg.type=mtype;
    msg.body.disconnect.sender=addrID;
    msg.len=sizeof(int)+sizeof(MType)+sizeof(address);;
    break;
  default:
    msg.body.def.problem_id=-1;
    perror("Error creating the message Msg");
    break;
  }

  return(msg);
}

