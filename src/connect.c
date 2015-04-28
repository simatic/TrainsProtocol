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
#include "trains.h"
#include "counter.h"

int openConnection(address addr, bool isPred){
  int rank;
  trComm * tcomm;

  rank = addrToRank(addr);
  if (rank == -1) {
    ERROR_AT_LINE_WITHOUT_ERRNUM(EXIT_FAILURE, __FILE__, __LINE__,
        "Wrong address %d sent to openConnection", addr);
    return (-1);
  } else {
    if (globalAddrArray[rank].chan[0] == '\0')
      return (-1);
    tcomm = commNewAndConnect(globalAddrArray[rank].ip,
        globalAddrArray[rank].chan, CONNECT_TIMEOUT);
    if (tcomm == NULL )
      return (-1);
    else {
      pthread_t thread;
      int rc;
      addTComm(tcomm, rank, globalAddrArray, isPred);
      rc = pthread_create(&thread, NULL, &connectionMgt, (void *) tcomm);
      if (rc < 0)
        ERROR_AT_LINE(EXIT_FAILURE, rc, __FILE__, __LINE__, "pthread_create");
      rc = pthread_detach(thread);
      if (rc < 0)
        ERROR_AT_LINE(EXIT_FAILURE, rc, __FILE__, __LINE__, "pthread_detach");
      return (1);
    }
  }
}

void closeConnection(address addr, bool isPred){
  int rank;
  trComm * tcomm;

  rank = addrToRank(addr);
  if (rank == -1){
    ERROR_AT_LINE_WITHOUT_ERRNUM(EXIT_FAILURE, __FILE__, __LINE__,
        "Wrong address %d sent to closeConnection", addr);
  } else {
    tcomm = getTComm(rank, isPred, globalAddrArray);
    if (tcomm != NULL ) {
      removeTComm(tcomm, rank, globalAddrArray);
      freeComm(tcomm);
    }
  }
}

address searchSucc(address add){
  int i;
  int watch = 1;
  int rank;
  address result = myAddress;

  rank = addrToRank(add);
  if (rank == -1){
    ERROR_AT_LINE_WITHOUT_ERRNUM(EXIT_FAILURE, __FILE__, __LINE__,
        "Wrong address %d given to searchSucc", add);
  } else {
    i = (rank + 1) % NP;
    while (i != rank && watch) {
      if (openConnection(rankToAddr(i), false) == 1) {
        result = rankToAddr(i);
        watch = 0;
      } else {
        i = (i + 1) % NP;
      }
    }
  }
  return (result);
}

void *connectionMgt(void *arg){
  pthread_t treatmentThread;
  trCommAndQueue *commAndQueue;
  trBqueue *msgQueue = newBqueue();
  trComm *aComm = (trComm*) arg;
  womim * msgExt;
  int rc;
  bool b;

  commAndQueue = malloc(sizeof(trCommAndQueue));
  assert(commAndQueue != NULL);

  commAndQueue->aComm = aComm;
  commAndQueue->msgQueue = msgQueue;

  rc = pthread_create(&treatmentThread, NULL, &msgTreatment,
      (void *) commAndQueue);
  if (rc < 0)
    ERROR_AT_LINE(EXIT_FAILURE, rc, __FILE__, __LINE__, "pthread_create");
  rc = pthread_detach(treatmentThread);
  if (rc < 0)
    ERROR_AT_LINE(EXIT_FAILURE, rc, __FILE__, __LINE__, "pthread_detach");

  do {
    msgExt = receive(aComm);
    b = ((msgExt != NULL ) &&
        (msgExt->msg.type != DISCONNECT_PRED)&&
        (msgExt->msg.type != DISCONNECT_SUCC));
    bqueueEnqueue(msgQueue, msgExt);
  } while (b);
  // NB : The test cannot be
  //} while (msgExt->msg.type != DISCONNECT);
  // because msg.typ is freed inside stateMachine()

  return NULL ;
}

void *msgTreatment(void *arg){
  trCommAndQueue *commAndQueue = (trCommAndQueue*) arg;
  trBqueue *msgToTreatQueue = commAndQueue->msgQueue;
  trComm *aComm = commAndQueue->aComm;
  womim *msgExt;
  bool theEnd = false;

  do {
    msgExt = bqueueDequeue(msgToTreatQueue);
    if (msgExt == NULL ) {
      break;
    }
    switch (msgExt->msg.type) {
    case TRAIN:
      counters.trains_received++;
      counters.trains_bytes_received += msgExt->msg.len;
      break;
    case INSERT:
      addTComm(aComm, addrToRank(msgExt->msg.body.insert.sender),
          globalAddrArray, true);
      break;
    case NEWSUCC:
      addTComm(aComm, addrToRank(msgExt->msg.body.newSucc.sender),
          globalAddrArray, false);
      break;
    case DISCONNECT_PRED:
    case DISCONNECT_SUCC:
      theEnd = true;
      break;
    default:
      break;
    }
    stateMachine(msgExt);
  } while (!theEnd);
  // NB : The test cannot be 
  //} while (msgExt->msg.type != DISCONNECT);
  // because msg.typ is freed inside stateMachine()

  free(commAndQueue);
  return NULL ;
}
