#include <stdlib.h>
#include <stdio.h>
#include <error.h>
#include <errno.h>
#include <assert.h>
#include <pthread.h>

#include "iomsg.h"



womim * receive(t_comm * aComm){
  womim * msg_ext;
  int nbRead, nbRead2;
  int length;
  int j;

  nbRead = comm_readFully(aComm, &length, sizeof(length));
  if (nbRead == sizeof(length)){
    msg_ext = calloc(length+sizeof(prefix),sizeof(char));
    assert(msg_ext != NULL);
    nbRead2 = comm_readFully(aComm, ((char*)msg_ext)+sizeof(prefix)+nbRead, (length-nbRead));
    if (nbRead2 == length-nbRead) {
      pthread_mutex_init(&(msg_ext->pfx.mutex),NULL);
      msg_ext->pfx.counter=1; 
      msg_ext->msg.len=length;
      return(msg_ext);
    } else {
      free(msg_ext);
    }
  }

  //Connection has been closed
  //search the address which has vanished
  j=search_tcomm(aComm,global_addr_array);
  if(j==-1){
    error_at_line(EXIT_FAILURE,0,__FILE__,__LINE__,"search_tcomm() return unexpected -1");
  }
  //create the DICONNECT to return
  msg_ext = calloc(sizeof(prefix)+sizeof(newMsg(DISCONNECT,rank_2_addr(j))),sizeof(char));
  pthread_mutex_init(&(msg_ext->pfx.mutex),NULL);
  msg_ext->pfx.counter=1;
  msg_ext->msg=newMsg(DISCONNECT,rank_2_addr(j));
  //close the connection
  close_connection(rank_2_addr(j));
  return(msg_ext);
}

//Use to sendall the messages Msg, even the TRAIN ones, but in fact, TRAIN messages will never be created for the sending, but use only on reception... Thus, to send TRAIN messages, send_train will be used.
//use global_addr_array defined in management_addr.h
int send_other(address addr, MType type, address sender){
  //int send_other(address addr, Msg * msg){
  int length;
  int iovcnt=1;
  struct iovec iov[1];
  int rank=-1;
  t_comm * aComm;
  int result;
  Msg * msg;

  if(type==TRAIN){
    error_at_line(EXIT_FAILURE,errno,__FILE__,__LINE__,"Wrong MType given to send_other");
    return(-1);
  }
  else{
    msg=malloc(sizeof(newMsg(type,sender)));
    *msg=newMsg(type,sender);

    length=msg->len;    
    rank=addr_2_rank(addr);
    if(rank!=-1)
      {
	aComm=global_addr_array[rank].tcomm;
	iov[0].iov_base=msg;
	iov[0].iov_len=length;
	result=comm_writev(aComm,iov,iovcnt);
	if(result!=length)
	  fprintf(stderr, "result!=length\n");
	free(msg);
	return(result);
      }
    else{
      //should return an error if the addr is out of rank
      free(msg);
      error_at_line(EXIT_FAILURE,errno,__FILE__,__LINE__,"Sending failure in send_other");
      return(-1);//same error as comm_writev !!
    }
  }
}

//send a train -> use send_other to send the rest
int send_train(address addr, lts_struct lts){
  int global_length=0;
  MType tosend=TRAIN; 
  int iovcnt=8;
  struct iovec iov[8];
  int rank=-1;
  t_comm * aComm;
  int result;
  
  rank=addr_2_rank(addr);
  if(rank!=-1)
    {
      aComm=global_addr_array[rank].tcomm;
      global_length=
	sizeof(int)+
	sizeof(MType)+
	3*sizeof(char)+
	sizeof(address_set)+
	lts.w.len;
      //to begin, let's enter the length of the message
      iov[0].iov_base=&global_length;
      iov[0].iov_len=sizeof(int);
      //first loading the type of Message (MType)
      iov[1].iov_base=&tosend;
      iov[1].iov_len=sizeof(MType);
      //then loading of the train's stramp and circuit
      iov[2].iov_base=&(lts.stamp.id);
      iov[2].iov_len=sizeof(char);
      iov[3].iov_base=&(lts.stamp.lc);
      iov[3].iov_len=sizeof(char);
      iov[4].iov_base=&(lts.stamp.round);
      iov[4].iov_len=sizeof(char);
      iov[5].iov_base=&(lts.circuit);
      iov[5].iov_len=sizeof(address_set);
      //after loading the wagons
      iov[6].iov_base=lts.w.w_w->p_wagon;
      iov[6].iov_len=lts.w.len;
      //finally loading the wagon which is waiting to be sent
      if(lts.p_wtosend == NULL){
	iov[7].iov_base=NULL;
	iov[7].iov_len=0;
	result=comm_writev(aComm,iov,iovcnt);
      }
      else {
	global_length = global_length + lts.p_wtosend->p_wagon->header.len;
	iov[7].iov_base=lts.p_wtosend->p_wagon;
	iov[7].iov_len=lts.p_wtosend->p_wagon->header.len;
	//sending the whole train with writev
	//returning the number of bytes send
	result=comm_writev(aComm,iov,iovcnt);
      }
      if(result!=global_length)
	fprintf(stderr, "result!=length (bis)\n");
      return(result);
    }
  else{
    //should return an error if the addr is out of rank
    error_at_line(EXIT_FAILURE,errno,__FILE__,__LINE__,"Sending failure in send_other");
    return(-1);//same error as comm_writev !!
  }
}


