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

#include <stdio.h>
#include <time.h>
#include "stateMachine.h"
#include "interface.h"
#include "advanced_struct.h"
#include "connect.h"
#include "counter.h"

State automatonState=OFFLINE_CONNECTION_ATTEMPT;
pthread_mutex_t stateMachineMutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;
address myAddress;
address prec;
address succ;
addressSet cameProc=0;
addressSet goneProc=0;
int waitNb=0;
int lis; //last id sent
ltsArray lts; //last trains sent
t_list* unstableWagons[MAX_NTR][NR];
t_bqueue* wagonsToDeliver;

void stateMachine (womim* p_womim);
void nextState (State s);

char *stateToStr(State state) {
  static char s[64];
  switch(state){
  case OFFLINE_CONNECTION_ATTEMPT:
    return "OFFLINE_CONNECTION_ATTEMPT";
  case OFFLINE_CONFIRMATION_WAIT:
    return "OFFLINE_CONFIRMATION_WAIT";
  case ALONE_INSERT_WAIT:
    return "ALONE_INSERT_WAIT";
  case ALONE_CONNECTION_WAIT:
    return "ALONE_CONNECTION_WAIT";
  case SEVERAL:
    return "SEVERAL";
  case   WAIT:
    return "WAIT";
  default:
    sprintf(s, "Unknown (value = %d)", state);
    return s;
  }
}

char *msgTypeToStr(MType mtype){
  static char s[64];
  switch(mtype){
  case DEFAULT:
    return "DEFAULT";
  case TRAIN:
    return "TRAIN";
  case INSERT:
    return "INSERT";
  case ACK_INSERT:
    return "ACK_INSERT";
  case NAK_INSERT:
    return "NAK_INSERT";
  case DISCONNECT_PRED:
    return "DISCONNECT_PRED";
  case DISCONNECT_SUCC:
    return "DISCONNECT_SUCC";
  case NEWSUCC:
    return "NEWSUCC";
  default:
    sprintf(s, "Unknown (value = %d)", mtype);
    return s;
  }
}

void *acceptMgt(void *arg) {
  t_comm *commForAccept = (t_comm*)arg;
  t_comm *aComm;

  do{
    aComm = commAccept(commForAccept);
    if (aComm != NULL){
      // We fork a thread responsible for handling this connection
      pthread_t thread;
      int rc = pthread_create(&thread, NULL, &connectionMgt, (void *)aComm);
      if (rc < 0)
	error_at_line(EXIT_FAILURE, rc, __FILE__, __LINE__, "pthread_create");
      rc = pthread_detach(thread);
      if (rc < 0)
	error_at_line(EXIT_FAILURE, rc, __FILE__, __LINE__, "pthread_detach");
    }
  } while (aComm != NULL);

  return NULL;
}


void participate(bool b){
  static t_comm *commForAccept = NULL;
  pthread_t thread;

  if (b){
    commForAccept = commNewForAccept(globalAddrArray[addrToRank(myAddress)].chan);
    if (commForAccept == NULL)
      error_at_line(EXIT_FAILURE, errno, __FILE__, __LINE__, "comm_newForAccept");
    int rc = pthread_create(&thread, NULL, &acceptMgt, (void *)commForAccept);
    if (rc < 0)
      error_at_line(EXIT_FAILURE, rc, __FILE__, __LINE__, "pthread_create");
    rc = pthread_detach(thread);
    if (rc < 0)
      error_at_line(EXIT_FAILURE, rc, __FILE__, __LINE__, "pthread_detach");
  } else {
    freeComm(commForAccept);
  }
}


void automatonInit () {
  int id;
  int round;
  lis=ntr-1;
  for (id=0;id<ntr;id++){
    //(lts[id]).lng is not initialized because lng is only set in sendTrain()
    (lts[id]).type=TRAIN;
    (lts[id]).stamp.id=id;
    (lts[id]).stamp.lc=0;
    (lts[id]).stamp.round=0;
    (lts[id]).circuit=0;
    (lts[id]).w.w_w.p_wagon=NULL;
    (lts[id]).w.w_w.p_womim=NULL;
    (lts[id]).w.len=0;
    (lts[id]).p_wtosend=NULL;
    for (round=0;round<NR;round++) {
      unstableWagons[id][round]=newList();
    }
  }
  cameProc=0;
  goneProc=0;
  wagonToSend = newWiw();
  wagonsToDeliver=newBqueue();
    
  prec=0;
  succ=0;
  MUTEX_LOCK(stateMachineMutex);
  nextState(OFFLINE_CONNECTION_ATTEMPT);
  MUTEX_UNLOCK(stateMachineMutex);
}

void trainHandling(womim *p_womim) {
  int id = p_womim->msg.body.train.stamp.id;
  int round = p_womim->msg.body.train.stamp.round;
  wagon *p_wag;

  counters.recent_trains_received++;
  counters.recent_trains_bytes_received += p_womim->msg.len;
  if (round==lts[id].stamp.round)
    {
      round=(round+1) % NR;
    }
  bqueueExtend(wagonsToDeliver,unstableWagons[id][(round-2+NR) % NR]);
  cleanList(unstableWagons[id][(round-2+NR) % NR]);
  if (id==0)
    {
      lts[0].circuit = addrUpdateCircuit(p_womim->msg.body.train.circuit,myAddress,cameProc,goneProc);
      cameProc=0;
      signalDepartures(goneProc, lts[0].circuit);
      goneProc=0;
    }
  else
    {
      lts[id].circuit=lts[0].circuit;
    }

  releaseWiw(&(lts[id].w.w_w));
  lts[id].w.len=0;
  freeWiw(lts[id].p_wtosend);
  lts[id].p_wtosend = NULL;

  for (p_wag=firstWagon(&(p_womim->msg)) ; p_wag!=NULL ; p_wag=nextWagon(p_womim,p_wag))
    {
      if (addrIsMember(p_wag->header.sender,lts[id].circuit)
	  && !(addrIsMember(p_wag->header.sender,goneProc))
	  && !(addrIsMine(p_wag->header.sender)))
	{
	  // We add a wiw (corresponding to this p_wag) to unstableWagons[id][round]
	  wiw *wi = malloc(sizeof(wiw));
	  assert(wi != NULL);
	  wi->p_wagon = p_wag;
	  wi->p_womim = p_womim;
	  MUTEX_LOCK(p_womim->pfx.mutex);
	  p_womim->pfx.counter++;
	  MUTEX_UNLOCK(p_womim->pfx.mutex);
	  listAppend(unstableWagons[id][round],wi);

	  // Shall this wagon be sent to our successor
	  if(!addrIsEqual(succ,p_wag->header.sender))
	    {
	      // Yes, it must sent
	      if(lts[id].w.w_w.p_wagon == NULL)
		{
		  // lts must be updated to point on the first wagon to be sent
		  lts[id].w.w_w.p_wagon=p_wag;
		  lts[id].w.w_w.p_womim=p_womim;
		  MUTEX_LOCK(lts[id].w.w_w.p_womim->pfx.mutex);
		  lts[id].w.w_w.p_womim->pfx.counter++;
		  MUTEX_UNLOCK(lts[id].w.w_w.p_womim->pfx.mutex);
		}
	      // We adapt len
	      lts[id].w.len += p_wag->header.len;
	    }
	}
    }

  lts[id].stamp.round=(char)round;  
  MUTEX_LOCK(mutexWagonToSend);
  if (firstMsg(wagonToSend->p_wagon) != NULL) {
    wagonToSend->p_wagon->header.round=round;

    // We add a wiw (corresponding to this p_wag) to wagonToSend
    wiw *wi = malloc(sizeof(wiw));
    assert(wi != NULL);
    *wi = *wagonToSend;
    // We do not need to lock wagonToSend->p_wagon->pfx.mutex
    // as we are for the moment alone to access to 
    // wagonToSend->p_wagon->pfx.counter
    wagonToSend->p_womim->pfx.counter++;
    listAppend(unstableWagons[id][round],wi);

    lts[id].p_wtosend = wagonToSend;

    wagonToSend = newWiw();
  }
  MUTEX_UNLOCK(mutexWagonToSend);
  pthread_cond_signal(&condWagonToSend);

  lts[id].stamp.lc = p_womim->msg.body.train.stamp.lc + 1;
}

int randSleep(int nbwait) {
  srand(time(NULL));
  int i = rand()%(1<<(nbwait));
  return i;
}

void waitBeforConnect () {
  if (waitNb>waitNbMax) {
    error_at_line(EXIT_FAILURE, errno, __FILE__, __LINE__, "wait_nb_max owerflowed");
  }
  participate(false);
  closeConnection(prec,true);
  closeConnection(succ,false);
  prec=0;
  succ=0;
  int sleep=randSleep(waitNb)*waitDefaultTime;
  printf("waiting time : %d\n",sleep);
  usleep(sleep);
  waitNb++;
  return;
}

void offlineInit () {
participate(true);
    succ=searchSucc(myAddress);
    if (addrIsMine(succ))
      {
	signalArrival(myAddress, (addressSet)myAddress);
	bqueueEnqueue(wagonsToDeliver,wagonToSend);
	wagonToSend=newWiw();
	automatonState=ALONE_INSERT_WAIT;
	//printf("Nextstate(fake) = %s\n", stateToStr(automatonState));
	int rc=sem_post(&sem_init_done);
	if (rc)
	  error_at_line(EXIT_FAILURE,errno,__FILE__,__LINE__,"error in sem_post");
	prec=myAddress;
	return;
      }
    sendOther(succ,false, INSERT, myAddress);
    automatonState=OFFLINE_CONNECTION_ATTEMPT;
    //printf("Nextstate(fake) = %s\n", stateToStr(automatonState));
    return;
}


void nextState (State s) {
  //printf("Nextstate = %s\n", stateToStr(s));
  switch (s) {
  case OFFLINE_CONNECTION_ATTEMPT :
    offlineInit();
    break;
  case OFFLINE_CONFIRMATION_WAIT :
    automatonState=OFFLINE_CONFIRMATION_WAIT;
    break;
  case WAIT :
    waitBeforConnect();
    offlineInit();
    break;
  case ALONE_INSERT_WAIT :
    prec=myAddress;
    succ=myAddress;
    automatonState=ALONE_INSERT_WAIT;
    break;
  case ALONE_CONNECTION_WAIT :
    automatonState=ALONE_CONNECTION_WAIT;
    break;
  case SEVERAL :
    automatonState=SEVERAL;
    break;
  default :
    error_at_line(EXIT_FAILURE, errno, __FILE__, __LINE__, "unexpected Automaton State : %d",s);
    break;
  }
}

void stateMachine (womim* p_womim) {
  int id;
  //printf("State = %s, receive message = %s\n", stateToStr(automatonState), msgTypeToStr(p_womim->msg.type));
  MUTEX_LOCK(stateMachineMutex);
  switch (automatonState)
    {
    case OFFLINE_CONNECTION_ATTEMPT :
      switch (p_womim->msg.type)
	{
	case NAK_INSERT :
	case DISCONNECT_PRED :
	case DISCONNECT_SUCC :
	  freeWomim(p_womim);
	  nextState(WAIT);
	  break;
	case INSERT :
	  sendOther(p_womim->msg.body.insert.sender,true, NAK_INSERT, myAddress);
	  freeWomim(p_womim);
	  break;
	case ACK_INSERT :
	  prec=p_womim->msg.body.ackInsert.sender;
	  openConnection(prec,true);
	  sendOther(prec,true, NEWSUCC, myAddress);
	  freeWomim(p_womim);	  
	  nextState(OFFLINE_CONFIRMATION_WAIT);
	  break;
	default :
	  error_at_line(EXIT_FAILURE, errno, __FILE__, __LINE__, "unexpected case : received message %s in state %s",msgTypeToStr(p_womim->msg.type),stateToStr(automatonState));
	  break;
	}
      break;

    case OFFLINE_CONFIRMATION_WAIT :
      switch (p_womim->msg.type)
	{
	case NEWSUCC :
	case DISCONNECT_PRED :
	case DISCONNECT_SUCC :
	  freeWomim(p_womim);	  
	  nextState(WAIT);
	  break;
	case INSERT :
	  sendOther(p_womim->msg.body.insert.sender,true, NAK_INSERT, myAddress);
	  freeWomim(p_womim);	  
	  break;
	case TRAIN :
	  id = (int)p_womim->msg.body.train.stamp.id;
	  if (addrIsMember(myAddress,p_womim->msg.body.train.circuit))
	    {
	      p_womim->msg.body.train.stamp.lc++;
	    }
	  releaseWiw(&(lts[id].w.w_w));
	  lts[id].w.len = 0;
	  if (firstWagon(&(p_womim->msg)) != NULL) {
	    lts[id].w.w_w.p_wagon=firstWagon(&(p_womim->msg));
	    lts[id].w.w_w.p_womim=p_womim;
	    lts[id].w.len=(p_womim->msg.len-sizeof(stamp)-sizeof(int)-sizeof(MType)-sizeof(address));
	    // We do not need to lock the p_womim->pfx.mutex as we are the only thread
	    // accessing to counter
	    p_womim->pfx.counter++;
	  }
	  lts[id].circuit=p_womim->msg.body.train.circuit;
	  lts[id].stamp=p_womim->msg.body.train.stamp;
	  sendTrain(succ,false,lts[id]);
	  if (isInLts(myAddress,lts))
	    {
	      lis=p_womim->msg.body.train.stamp.id;
	      signalArrival(myAddress, lts[lis].circuit);
	      nextState(SEVERAL);
	      int rc=sem_post(&sem_init_done);
	      if (rc)
		error_at_line(EXIT_FAILURE,errno,__FILE__,__LINE__,"error in sem_post");
	    }
	  freeWomim(p_womim);
	  break;
	default :
	  error_at_line(EXIT_FAILURE, errno, __FILE__, __LINE__, "unexpected case : received message %s in state %s",msgTypeToStr(p_womim->msg.type),stateToStr(automatonState));
	  freeWomim(p_womim);	  
	  break;
	}
      break;

    case WAIT :
      error_at_line(EXIT_FAILURE, errno, __FILE__, __LINE__, "unexpected reception while WAIT");
      freeWomim(p_womim);	  
      break;
      
    case ALONE_INSERT_WAIT :
      switch (p_womim->msg.type)
	{
	case DISCONNECT_PRED :
	case DISCONNECT_SUCC :
	  // May happen in case there are 2 processes on the circuit and one process dies
	  // The other process receives DISCONNECT in SEVERAL state and switched to 
	  // ALONE_INSERT_WAIT state, where it receives the DISCONNECT corresponding
	  // to the loss of the second connection
	  freeWomim(p_womim);	  
	  break;
	case INSERT :
	  MUTEX_LOCK(mutexWagonToSend);
	  sendOther(p_womim->msg.body.insert.sender,true,ACK_INSERT, myAddress);
	  prec=p_womim->msg.body.insert.sender;
	  freeWomim(p_womim);	  
	  nextState(ALONE_CONNECTION_WAIT);
	  MUTEX_UNLOCK(mutexWagonToSend);
	  break;
	default :
	  error_at_line(EXIT_FAILURE, errno, __FILE__, __LINE__, "unexpected case : received message %s in state %s",msgTypeToStr(p_womim->msg.type),stateToStr(automatonState));
	  freeWomim(p_womim);	  
	  break;
	}
      break;

    case ALONE_CONNECTION_WAIT :
      switch (p_womim->msg.type)
	{
	case DISCONNECT_PRED :
	  nextState(ALONE_INSERT_WAIT);
	  freeWomim(p_womim);	  
	  break;
	case INSERT :
	  // In the PhD-thesis algorithm, we are supposed to do a
	  // saveUntilNextstate(p_womim). Sending a NAK_INSERT is more simple.
	  sendOther(p_womim->msg.body.insert.sender,true,NAK_INSERT, myAddress);
	  freeWomim(p_womim);
	  break;
	case NEWSUCC :
	  succ=p_womim->msg.body.newSucc.sender;
	  int i;
	  for (i=1;i<=ntr;i++) {
	    int id=((lis+i) % ntr);
	    (lts[(int)id]).stamp.lc+=1;
	    (lts[(int)id]).circuit=myAddress | prec;
	    (lts[(int)id]).w.w_w.p_wagon=NULL;
	    (lts[(int)id]).w.w_w.p_womim=NULL;
	    (lts[(int)id]).w.len=0;
	    sendTrain(succ,false,lts[(int)id]);
	  }
	  freeWomim(p_womim);	  
	  nextState(SEVERAL);
	  break;
	default :
	  error_at_line(EXIT_FAILURE, errno, __FILE__, __LINE__, "unexpected case : received message %s in state %s",msgTypeToStr(p_womim->msg.type),stateToStr(automatonState));
	  freeWomim(p_womim);	  
	  break;
	}
      break;
      
    case SEVERAL :
      switch (p_womim->msg.type)
	{
	case TRAIN :
	  if (addrIsMember(myAddress,p_womim->msg.body.train.circuit))
	    {
	      if (isRecentTrain(p_womim->msg.body.train.stamp,lts,lis))
		{
		  trainHandling(p_womim);
		  lis=p_womim->msg.body.train.stamp.id;
		  sendTrain(succ,false,lts[lis]);
		}
	    }
	  else
	    {
	      error_at_line(EXIT_FAILURE, 0, __FILE__, __LINE__, "my_address not in the circuit ==> Suicide");
	    }
	  freeWomim(p_womim);	  
	  break;
	case INSERT :
	  closeConnection(prec,true);
	  sendOther(p_womim->msg.body.insert.sender,true,ACK_INSERT, prec);
	  prec=p_womim->msg.body.insert.sender;
	  addrAppendArrived(&cameProc,prec);
	  freeWomim(p_womim);	  
	  break;
	case NEWSUCC :
	  closeConnection(succ,false);
	  succ=p_womim->msg.body.newSucc.sender;
	  int i;
	  for (i=1;i<=ntr;i++)
	    {
	      sendTrain(succ,false,lts[(lis+i) % ntr]);
	    }
	  freeWomim(p_womim);	  
	  break;
	case DISCONNECT_PRED :
	  MUTEX_LOCK(mutexWagonToSend);
	  while (!(addrIsEqual(prec,myAddress)))
	    {
	      if (openConnection(prec,true)!=(-1))
		{
		  sendOther(prec,true,NEWSUCC, myAddress);
		  freeWomim(p_womim);	  
		  nextState(SEVERAL);
		  MUTEX_UNLOCK(stateMachineMutex);
		  MUTEX_UNLOCK(mutexWagonToSend);
		  return;
		}
	      addrAppendGone(&cameProc,&goneProc,prec);
	      prec=addrPrec(prec, lts[lis].circuit);
	    }
	  signalDepartures(goneProc, lts[lis].circuit);
	  goneProc=0;
	  int aRound;
	  for (aRound=1;aRound<=NR;aRound++)
	    {
	      int i;
	      for (i=1;i<=ntr;i++)
		{
		  int id=(lis+i) % ntr;
		  int round=(lts[id].stamp.round + aRound) % NR;
		  bqueueExtend(wagonsToDeliver,unstableWagons[id][round]);
		  cleanList(unstableWagons[id][round]);
		}
	    }
	  bqueueEnqueue(wagonsToDeliver,wagonToSend);
	  wagonToSend=newWiw();
	  freeWomim(p_womim);	  
	  nextState(ALONE_INSERT_WAIT);
	  MUTEX_UNLOCK(stateMachineMutex);
	  MUTEX_UNLOCK(mutexWagonToSend);
	  break;
	case DISCONNECT_SUCC :
	  succ=0;
	  freeWomim(p_womim);	  
	  break;
	default:
	  error_at_line(EXIT_FAILURE, errno, __FILE__, __LINE__, "unexpected case : received message %s in state %s",msgTypeToStr(p_womim->msg.type),stateToStr(automatonState));
      break;
	}
       break;
    default :
      error_at_line(EXIT_FAILURE, 0, __FILE__, __LINE__, "Unknown state : %d",automatonState);
      freeWomim(p_womim);	  
      break;
    }
  MUTEX_UNLOCK(stateMachineMutex);
}
