#include <stdlib.h>
#include <pthread.h>

#include "connect.h"
#include "advanced_struct.h"
#include "iomsg.h"
#include "stateMachine.h"
#include "param.h"

void *connectionMgt(void *arg) {
  t_comm *aComm = (t_comm*)arg;
  womim * msg_ext;

  do{
    msg_ext = receive(aComm);
    switch(msg_ext->msg.type){
    case INSERT:
      add_tcomm(aComm, addr_2_rank(msg_ext->msg.body.insert.sender), global_addr_array);
      break;
    case NEWSUCC:
      add_tcomm(aComm, addr_2_rank(msg_ext->msg.body.newSucc.sender), global_addr_array);
      break;
    default:
      break;
    }
    stateMachine(msg_ext);
  } while (msg_ext->msg.type != DISCONNECT);

  return NULL;
}

int open_connection(address addr){
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
      add_tcomm(tcomm,rank,global_addr_array);
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

void close_connection(address addr){
  int rank;
  t_comm * tcomm;
  
  rank=addr_2_rank(addr);
  if (rank==-1)
    error_at_line(EXIT_FAILURE,0,__FILE__,__LINE__,"Wrong address %d sent to close_connection",addr);
  else{
    tcomm=global_addr_array[rank].tcomm;
    if(tcomm==NULL)
      error_at_line(EXIT_FAILURE,0,__FILE__,__LINE__,"Wrong address sent to close_connection");
    else{
      comm_abort(tcomm);
      global_addr_array[rank].tcomm=NULL;
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
      if(open_connection(rank_2_addr(i))==1){
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
