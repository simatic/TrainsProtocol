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
#include <pthread.h>

#include "connect.h"
#include "advanced_struct.h"
#include "iomsg.h"
#include "stateMachine.h"
#include "param.h"
#include "counter.h"

int open_connection(address addr, bool isPred){
  int rank;
  t_comm * tcomm;

  rank=addr_2_rank(addr);
  if (rank==-1){
    error_at_line(EXIT_FAILURE,0,__FILE__,__LINE__,"Wrong address %d sent to open_connection",addr);
    return(-1);
  }
  else{
    tcomm=comm_newAndConnect(global_addr_array[rank].ip,global_addr_array[rank].chan,CONNECT_TIMEOUT);
    if (tcomm==NULL)
      return(-1);
    else{
      pthread_t thread;
      int rc;
      add_tcomm(tcomm,rank,global_addr_array,isPred);
      rc = pthread_create(&thread, NULL, &connectionMgt, (void *)tcomm);
      if (rc < 0)
        error_at_line(EXIT_FAILURE, rc, __FILE__, __LINE__, "pthread_create");
      rc = pthread_detach(thread);
      if (rc < 0)
        error_at_line(EXIT_FAILURE, rc, __FILE__, __LINE__, "pthread_detach");
      return(1);
    }
  }
}

void close_connection(address addr, bool isPred){
  int rank;
  t_comm * tcomm;
  
  rank=addr_2_rank(addr);
  if (rank==-1)
    error_at_line(EXIT_FAILURE,0,__FILE__,__LINE__,"Wrong address %d sent to close_connection",addr);
  else{
    tcomm=get_tcomm(rank,isPred,global_addr_array);
    if(tcomm!=NULL){
      remove_tcomm(tcomm, rank, global_addr_array);
      comm_abort(tcomm);
    }
  }           
}

address searchSucc(address add){
  int i;
  int watch=1;
  int rank;
  address result=my_address;

  rank=addr_2_rank(add);
  if(rank==-1)
    error_at_line(EXIT_FAILURE,0,__FILE__,__LINE__,"Wrong address %d given to searchSucc",add);
  else{
    i=(rank+1)%NP;
    while(i!=rank && watch){
      if(open_connection(rank_2_addr(i),false)==1){
	result=rank_2_addr(i);
	watch=0;
      }
      else{
	i = (i+1)%NP;
      }
    }
  }
  return(result);
}

void *msgTreatment(void *arg){
  t_commAndQueue *commAndQueue = (t_commAndQueue*)arg;
  t_bqueue *msgToTreatQueue = commAndQueue->msgQueue;
  t_comm *aComm = commAndQueue->aComm;
  womim *msg_ext;
  bool theEnd = false;

  do{
    msg_ext = bqueue_dequeue(msgToTreatQueue);
    if (msg_ext == NULL) {
      break;
    }
    switch(msg_ext->msg.type){
    case TRAIN:
      counters.trains_received++;
      counters.trains_bytes_received += msg_ext->msg.len;
      break;
    case INSERT:
      add_tcomm(aComm, addr_2_rank(msg_ext->msg.body.insert.sender), global_addr_array, true);
      break;
    case NEWSUCC:
      add_tcomm(aComm, addr_2_rank(msg_ext->msg.body.newSucc.sender), global_addr_array, false);
      break;
    case DISCONNECT_PRED:
    case DISCONNECT_SUCC:
      theEnd = true;
      break;
    default:
      break;
    }
    stateMachine(msg_ext);
  } while (!theEnd);
  // NB : The test cannot be 
  //} while (msg_ext->msg.type != DISCONNECT);
  // because msg.typ is freed inside stateMachine()

  free(commAndQueue);
  return NULL;
}

void *connectionMgt(void *arg) {
  pthread_t treatmentThread;
  t_commAndQueue *commAndQueue;
  t_bqueue *msgQueue = bqueue_new();
  t_comm *aComm = (t_comm*)arg;
  womim * msg_ext;
  int rc;

  commAndQueue = malloc(sizeof(t_commAndQueue));
  assert(commAndQueue != NULL);

  commAndQueue->aComm = aComm;
  commAndQueue->msgQueue = msgQueue;

  rc = pthread_create(&treatmentThread, NULL, &msgTreatment, (void *)commAndQueue);
  if (rc < 0)
    error_at_line(EXIT_FAILURE, rc, __FILE__, __LINE__, "pthread_create");
  rc = pthread_detach(treatmentThread);
  if (rc < 0)
    error_at_line(EXIT_FAILURE, rc, __FILE__, __LINE__, "pthread_detach");

  do{
    msg_ext = receive(aComm);
    bqueue_enqueue(msgQueue, msg_ext);
  } while(
        (msg_ext != NULL) &&
        (msg_ext->msg.type != DISCONNECT_PRED) &&
        (msg_ext->msg.type != DISCONNECT_SUCC));
  // NB : The test cannot be
  //} while (msg_ext->msg.type != DISCONNECT);
  // because msg.typ is freed inside stateMachine()

  return NULL;
}
