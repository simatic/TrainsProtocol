//#define to access to definition of PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP
// (Linux specific?)
#define _GNU_SOURCE

#include <stdio.h>
#include "stateMachine.h"
#include "interface.h"
#include "advanced_struct.h"
#include "connect.h"

State automatonState=OFFLINE_CONNECTION_ATTEMPT;
pthread_mutex_t state_machine_mutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;
address my_address;
address prec;
address succ;
address_set cameProc=0;
address_set goneProc=0;
int waitNb=0;
int lis; //last id sent
lts_array lts; //last trains sent
t_list* unstableWagons[MAX_NTR][NR];
t_bqueue* wagonsToDeliver;

void stateMachine (womim* p_womim);
void nextstate (State s);

char *state2str(State state) {
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

char *mtype2str(MType mtype){
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
  case DISCONNECT:
    return "DISCONNECT";
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
    aComm = comm_accept(commForAccept);
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
    commForAccept = comm_newForAccept(global_addr_array[addr_2_rank(my_address)].chan);
    if (commForAccept == NULL)
      error_at_line(EXIT_FAILURE, errno, __FILE__, __LINE__, "comm_newForAccept");
    int rc = pthread_create(&thread, NULL, &acceptMgt, (void *)commForAccept);
    if (rc < 0)
      error_at_line(EXIT_FAILURE, rc, __FILE__, __LINE__, "pthread_create");
    rc = pthread_detach(thread);
    if (rc < 0)
      error_at_line(EXIT_FAILURE, rc, __FILE__, __LINE__, "pthread_detach");
  } else {
    comm_free(commForAccept);
  }
}


void automatonInit () {
  int id;
  int round;
  lis=ntr-1;
  for (id=0;id<ntr;id++){
    (lts[id]).stamp.id=id;
    (lts[id]).stamp.lc=0;
    (lts[id]).stamp.round=0;
    (lts[id]).circuit=0;
    (lts[id]).w.w_w=newwiw();
    (lts[id]).w.len=0;
    (lts[id]).p_wtosend=NULL;
    for (round=0;round<NR;round++) {
      unstableWagons[id][round]=list_new();
    }
  }
  cameProc=0;
  goneProc=0;
  wagonToSend = newwiw();
  wagonsToDeliver=bqueue_new();
    
  prec=0;
  succ=0;
  nextstate(OFFLINE_CONNECTION_ATTEMPT);
}

void train_handling(womim *p_womim) {
  char id = p_womim->msg.body.train.stamp.id;
  char round = p_womim->msg.body.train.stamp.round;
  if (round==(lts)[(int)id].stamp.round)
    {
      round=(round+1) % NR;
    }
  bqueue_extend(wagonsToDeliver,unstableWagons[(int)id][(round-2+NR) % NR]);
  list_cleanList(unstableWagons[(int)id][(round-2+NR) % NR]);
  if ((int)id==0)
    {
      (lts)[0].circuit = addr_updateCircuit(p_womim->msg.body.train.circuit,my_address,cameProc,goneProc);
      cameProc=0;
      signalDepartures(goneProc, lts[0].circuit);
      goneProc=0;
    }
  else
    {
      lts[(int)id].circuit=lts[0].circuit;
    }
  lts[(int)id].w.len=0;
  free_wiw(lts[(int)id].w.w_w);

  wagon* p_wag=firstWagon(&(p_womim->msg));
  if (p_wag!=NULL)
    {
      bool listupdated=false; //booleen de tricheur
      do
	{
	  if (addr_ismember(p_wag->header.sender,lts[(int)id].circuit)
	      && !(addr_ismember(p_wag->header.sender,goneProc))
	      && !(addr_ismine(p_wag->header.sender)))
	    {
	      list_append(unstableWagons[(int)id][(int)round],p_wag);
	      if(addr_cmp(my_address,p_wag->header.sender))
		{
		  if(!(listupdated))
		    {
		      listupdated=true;
		      // lts is updated
		      lts[(int)id].w.w_w=newwiw();
		      lts[(int)id].w.w_w->p_wagon=p_wag;
		      lts[(int)id].w.w_w->p_womim=p_womim;
		      MUTEX_LOCK(lts[(int)id].w.w_w->p_womim->pfx.mutex);
		      lts[(int)id].w.w_w->p_womim->pfx.counter++;
		      MUTEX_UNLOCK(lts[(int)id].w.w_w->p_womim->pfx.mutex);
		    }
		  //len is updated
		  lts[(int)id].w.len+=p_wag->header.len;
		}
	    }
	}
      while(((p_wag=nextWagon(p_womim,p_wag))!=NULL));
    }
  lts[(int)id].stamp.round=round;
  wagonToSend->p_wagon->header.round=round;
  list_append(unstableWagons[(int)id][(int)round],wagonToSend);
  wiw *wiwtoadd=newwiw();
  wiwtoadd->p_wagon=wagonToSend->p_wagon;
  wiwtoadd->p_womim=wagonToSend->p_womim;
  MUTEX_LOCK(wiwtoadd->p_womim->pfx.mutex);
  wiwtoadd->p_womim->pfx.counter++;
  MUTEX_UNLOCK(wiwtoadd->p_womim->pfx.mutex);
  lts[(int)id].p_wtosend=wiwtoadd;
  wagonToSend=newwiw();
  lts[(int)id].stamp.lc++;
}

int rand_sleep(int nbwait) {
  srand(time(NULL));
  return rand()%(1<<(nbwait-1));
}

void waitBeforConnect () {
  if (waitNb>wait_nb_max) {
    error_at_line(EXIT_FAILURE, errno, __FILE__, __LINE__, "wait_nb_max owerflowed");
  }
  participate(false);
  close_connection(prec);
  close_connection(succ);
  prec=0;
  succ=0;
  usleep(rand_sleep(waitNb)*wait_default_time);
  waitNb++;
  return;
}

void nextstate (State s) {
  printf("Nextstate = %s\n", state2str(s));
  switch (s) {
  case OFFLINE_CONNECTION_ATTEMPT :
    participate(true);
    succ=searchSucc(my_address);
    if (addr_ismine(succ))
      {
	signalArrival(my_address, (address_set)my_address);
	bqueue_enqueue(wagonsToDeliver,wagonToSend);
	wagonToSend=newwiw();
	automatonState=ALONE_INSERT_WAIT;
	int rc=sem_post(&sem_init_done);
	if (rc)
	  error_at_line(EXIT_FAILURE,errno,__FILE__,__LINE__,"error in sem_post");
	prec=my_address;
	return;
      }
    send_other(succ,INSERT, my_address);
    automatonState=OFFLINE_CONNECTION_ATTEMPT;
    break;
  case OFFLINE_CONFIRMATION_WAIT :
    automatonState=OFFLINE_CONFIRMATION_WAIT;
    break;
  case WAIT :
    waitBeforConnect();
    automatonState=OFFLINE_CONNECTION_ATTEMPT;
    break;
  case ALONE_INSERT_WAIT :
    prec=my_address;
    succ=my_address;
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
  }
}

void stateMachine (womim* p_womim) {
  printf("State = %s, receive message = %s\n", state2str(automatonState), mtype2str(p_womim->msg.type));
  MUTEX_LOCK(state_machine_mutex);
  switch (automatonState)
    {
    case OFFLINE_CONNECTION_ATTEMPT :
      switch (p_womim->msg.type)
	{
	case NAK_INSERT :
	case DISCONNECT :
	  nextstate(WAIT);
	  MUTEX_UNLOCK(state_machine_mutex);
	  break;
	case INSERT :
	  send_other(p_womim->msg.body.insert.sender,NAK_INSERT, my_address);
	  break;
	case ACK_INSERT :
	  prec=p_womim->msg.body.ackInsert.sender;
	  open_connection(prec);
	  send_other(prec,NEWSUCC, my_address);
	  nextstate(OFFLINE_CONFIRMATION_WAIT);
	  break;
	default :
	  error_at_line(EXIT_FAILURE, errno, __FILE__, __LINE__, "unexpected case : received message %s in state %s",mtype2str(p_womim->msg.type),state2str(automatonState));
	  break;
	}
      break;

    case OFFLINE_CONFIRMATION_WAIT :
      switch (p_womim->msg.type)
	{
	case NEWSUCC :
	case DISCONNECT :
	  nextstate(WAIT);
	  MUTEX_UNLOCK(state_machine_mutex);
	  break;
	case INSERT :
	  send_other(p_womim->msg.body.insert.sender,NAK_INSERT, my_address);
	  break;
	case TRAIN :
	  if (addr_ismember(my_address,p_womim->msg.body.train.circuit))
	    {
	      p_womim->msg.body.train.stamp.lc++;
	    }
	  send_train(succ,lts[(int)p_womim->msg.body.train.stamp.id]);
	  if (is_in_lts(my_address,lts))
	    {
	      lis=p_womim->msg.body.train.stamp.id;
	      signalArrival(my_address, lts[lis].circuit);
	      nextstate(SEVERAL);
	      int rc=sem_post(&sem_init_done);
	      if (rc)
		error_at_line(EXIT_FAILURE,errno,__FILE__,__LINE__,"error in sem_post");
	    }
	  break;
	default :
	  error_at_line(EXIT_FAILURE, errno, __FILE__, __LINE__, "unexpected case : received message %s in state %s",mtype2str(p_womim->msg.type),state2str(automatonState));
	  break;
	}
      break;
    case WAIT :
      error_at_line(EXIT_FAILURE, errno, __FILE__, __LINE__, "unexpected reception while WAIT");
      break;
      
    case ALONE_INSERT_WAIT :
      switch (p_womim->msg.type)
	{
	case INSERT :
	  send_other(p_womim->msg.body.insert.sender,ACK_INSERT, my_address);
	  prec=p_womim->msg.body.insert.sender;
	  nextstate(ALONE_CONNECTION_WAIT);
	  break;
	default :
	  error_at_line(EXIT_FAILURE, errno, __FILE__, __LINE__, "unexpected case : received message %s in state %s",mtype2str(p_womim->msg.type),state2str(automatonState));
	  break;
	}
      break;

    case ALONE_CONNECTION_WAIT :
      switch (p_womim->msg.type)
	{
	case DISCONNECT :
	  prec=my_address;
	  break;
	case INSERT :
	  send_other(p_womim->msg.body.insert.sender,NAK_INSERT, my_address);
	  break;
	case NEWSUCC :
	  succ=p_womim->msg.body.newSucc.sender;
	  int i;
	  for (i=1;i<=ntr;i++) {
	    int id=((lis+i) % ntr);
	    (lts[(int)id]).stamp.lc++;
	    (lts[(int)id]).circuit=my_address | prec;
	    (lts[(int)id]).w.w_w=newwiw();
	    (lts[(int)id]).w.len=0;
	    send_train(succ,lts[(int)id]);
	  }
	  nextstate(SEVERAL);
	  break;
	default :
	  error_at_line(EXIT_FAILURE, errno, __FILE__, __LINE__, "unexpected case : received message %s in state %s",mtype2str(p_womim->msg.type),state2str(automatonState));
	  break;
	}
      break;
      
    case SEVERAL :
      switch (p_womim->msg.type)
	{
	case TRAIN :
	  if (addr_ismember(my_address,p_womim->msg.body.train.circuit))
	    {
	      if (is_recent_train(p_womim->msg.body.train.stamp,&lts,lis,ntr))
		{
		  train_handling(p_womim);
		  lis=p_womim->msg.body.train.stamp.id;
		  send_train(succ,lts[lis]);
		}
	    }
	  else
	    {
	      error_at_line(EXIT_FAILURE, 0, __FILE__, __LINE__, "my_address not in the circuit");
	    }
	  break;
	case INSERT :
	  close_connection(prec);
	  send_other(p_womim->msg.body.insert.sender,ACK_INSERT, my_address);
	  prec=p_womim->msg.body.insert.sender;
	  addr_appendArrived(&cameProc,prec);
	  break;
	case NEWSUCC :
	  close_connection(succ);
	  succ=p_womim->msg.body.newSucc.sender;
	  int i;
	  for (i=1;i<=ntr;i++)
	    {
	      send_train(succ,lts[(lis+i) % ntr]);
	    }
	  break;
	case DISCONNECT :
	  if (addr_isequal(p_womim->msg.body.disconnect.sender,prec))
	    {
	      while (!(addr_isequal(prec,my_address)))
		{
		  if (open_connection(prec)!=(-1))
		    {
		      send_other(prec,NEWSUCC, my_address);
		      nextstate(SEVERAL);
		      MUTEX_UNLOCK(state_machine_mutex);
		    }
		  addr_appendGone(&cameProc,&goneProc,prec);
		  prec=addr_prec(prec);
		}
	      signalDepartures(goneProc, lts[(int)lis].circuit);
	      goneProc=0;
	      int aRound;
	      int i;
	      for (aRound=1;aRound<=NR;aRound++)
		{
		  for (i=1;i<=ntr;i++)
		    {
		      int id=(lis+i) % ntr;
		      char round=(lts[id].stamp.round + aRound) % NR;
		      bqueue_extend(wagonsToDeliver,unstableWagons[id][(int)round]);
		      list_cleanList(unstableWagons[id][(int)round]);
		    }
		}
	      bqueue_enqueue(wagonsToDeliver,wagonToSend);
	      wagonToSend=newwiw();
	      nextstate(ALONE_INSERT_WAIT);
	      MUTEX_UNLOCK(state_machine_mutex);
	    }
	  else // connection lost with succ
	    {
	      succ=0;
	    }
	  break;
	default:
	  error_at_line(EXIT_FAILURE, errno, __FILE__, __LINE__, "unexpected case : received message %s in state %s",mtype2str(p_womim->msg.type),state2str(automatonState));
	}
      nextstate(SEVERAL);
      break;
    default :
      error_at_line(EXIT_FAILURE, 0, __FILE__, __LINE__, "Unknown state : %d",automatonState);
    }
  MUTEX_UNLOCK(state_machine_mutex);
}
